/**
 * @file transcode.c
 * @author Yu Zhong (lx1zhong@qq.com)
 * @brief 
 * @date 2022-08-29
 * 
 * @copyright Copyright (c) 2022
 * 
 */
// #define TIMER__

#include "transcode.h"
#include "zstd/common/fse.h"
#include <stdlib.h>

#ifdef TIMER__
#include <sys/time.h>

struct timeval time_start; /*only used for recording the cost*/
double timeCost = 0;


void time_cost_start()
{
	timeCost = 0;
	gettimeofday(&time_start, NULL);
}

void time_cost_end()
{
	double elapsed;
	struct timeval time_end;
	gettimeofday(&time_end, NULL);
	elapsed = ((time_end.tv_sec*1000000+time_end.tv_usec)-(time_start.tv_sec*1000000+time_start.tv_usec))/1000000.0;
	timeCost += elapsed;
}
#endif


/**
 * @brief transform type array to FseCode & tranCodeBits
 * [type] ---minus md---> [factor] ---transcode---> [tp_code]+[bitstream of diff]
 * 
 */
void encode_with_fse(int *type, size_t dataSeriesLength, unsigned int intervals, 
                    unsigned char **FseCode, size_t *FseCode_size, 
                    unsigned char **transCodeBits, size_t *transCodeBits_size) {

    // transcoding results of type array
    uint8_t *tp_code = (uint8_t *)malloc(dataSeriesLength);
    int nbits;

    BIT_CStream_t transCodeStream;
    int dstCapacity = dataSeriesLength * sizeof(int);
    (*transCodeBits) = (unsigned char *)malloc(dstCapacity);
    BIT_initCStream(&transCodeStream, (*transCodeBits), dstCapacity);

#ifdef TIMER__
	time_cost_start();
#endif
    // transcoding
    int md = intervals/2;

    // initialize array ahead to minimize the caculation in the long loop
    uint8_t type2code[intervals];
    unsigned int diff[intervals];
    for (int i=md; i<intervals; i++) {
        type2code[i] = (uint8_t)Int2code(i-md);
        diff[i] = i - md - code2int[type2code[i]][0];
    }
    for (int i=md-1; i>0; i--) {
        type2code[i] = TOTAL_CODE_NUM-type2code[2*md-i];
        diff[i] = diff[2*md-i];
    }

#ifdef TIMER__
    time_cost_end();
    printf("[adt-fse-0]: table time=%f\n", timeCost);

	time_cost_start();
#endif

    for (int i=0; i<dataSeriesLength; i++) {
        if (type[i] == 0) {
            // unpredictable data
            tp_code[i] = TOTAL_CODE_NUM;
            nbits = 0;
        }
        else {
            tp_code[i] = type2code[type[i]];
            nbits = code2int[tp_code[i]][1];
            BIT_addBitsFast(&transCodeStream, diff[type[i]], nbits);
            // printf(" %d: type=%d, nbits=%d, tp_code=%d, diff=%d\n", i, type[i], nbits, tp_code[i], diff[type[i]]);
            BIT_flushBitsFast(&transCodeStream);
        }
    }
    
#ifdef TIMER__
    time_cost_end();
    printf("[adt-fse-1]: transcode time=%f\n", timeCost);
    // fse encoding
    
	time_cost_start();
#endif

    // FSE
    (*FseCode) = (unsigned char*)malloc(2 * dataSeriesLength);
    /* ADT-HUFF0: if replace FSE_compress() with HUF_compress() (Canonical Huffman Encoding), 
       then our scheme becomes ADT-HUFF0, which provides slightly lower but similair performance than ADT-FSE.
       Remember to enlarge HUF_BLOCKSIZE_MAX in zstd/common/huf.h to enable large-block compression.*/
    // size_t fse_size = HUF_compress((*FseCode), 2 * dataSeriesLength, tp_code, dataSeriesLength);
    // if (HUF_isError(fse_size)) {
    size_t fse_size = FSE_compress((*FseCode), 2 * dataSeriesLength, tp_code, dataSeriesLength);
    if (FSE_isError(fse_size)) {
        printf("encode:FSE_isError!\n");
        // exit(1);
    }
    printf("fse_size=%lu, ", fse_size);
    (*FseCode_size) = fse_size;

    size_t const streamSize = BIT_closeCStream(&transCodeStream);
    printf("streamSize=%lu\n", streamSize);
    (*transCodeBits_size) = streamSize;
    if (streamSize == 0) {
        printf("too small!\n");   /* not enough space */
        exit(1);
    }
    
#ifdef TIMER__
    time_cost_end();
    printf("[adt-fse-2]: fse time=%f\n", timeCost);
#endif

    // FILE *f0 = fopen("/home/zhongyu/tmp/type_array.bin","wb");
    // fwrite(type, sizeof(int), dataSeriesLength, f0);
    // fclose(f0);

    free(tp_code);
}

/**
 * @brief transform FseCode & tranCodeBits to type array 
 * 
 */
void decode_with_fse(int *type, size_t dataSeriesLength, unsigned int intervals, 
                    unsigned char *FseCode, size_t FseCode_size, 
                    unsigned char *transCodeBits, size_t transCodeBits_size) {
#ifdef TIMER__
	time_cost_start();
#endif

    uint8_t *tp_code = (uint8_t *)malloc(dataSeriesLength);

    // fse
    if (FseCode_size <= 1) {
        // all 0
        memset((void *)type, 0, sizeof(int) * dataSeriesLength);
        return;
    }
    /* ADT-HUFF0: there're problems directly calling HUF_decompress() because of decompress size limit.*/
    // size_t fse_size = HUF_decompress(tp_code, dataSeriesLength, FseCode, FseCode_size);
    // if (HUF_isError(fse_size)) {
    size_t fse_size = FSE_decompress(tp_code, dataSeriesLength, FseCode, FseCode_size);
    if (FSE_isError(fse_size)) {
        printf("decode:FSE_isError!\n");
        // exit(1);
    }
    if (fse_size != dataSeriesLength) {
        printf("fse_size(%lu) != dataSeriesLength(%lu)!\n", fse_size, dataSeriesLength);
        exit(1);
    }
#ifdef TIMER__
    time_cost_end();
    printf("[adt-fse-0]: fse decode time=%f\n", timeCost);
    
	time_cost_start();
#endif
    BIT_DStream_t transCodeStream;
    size_t stream_size = BIT_initDStream(&transCodeStream, transCodeBits, transCodeBits_size);
    if (stream_size == 0) {
        printf("transcode stream empty!\n");
        // exit(1);
    }

    int md = intervals / 2;

    int code2type[TOTAL_CODE_NUM];
    for (int i=0; i<TOTAL_CODE_NUM; i++) {
        code2type[i] = code2int[i][0] + md;
    }
#ifdef TIMER__
    time_cost_end();
    printf("[adt-fse-1]: table time=%f\n", timeCost);
    
	time_cost_start();
#endif

    // detranscode
    int nbits;
    size_t diff;
    for (int i=dataSeriesLength-1; i>=0; i--) {
        if (tp_code[i] == TOTAL_CODE_NUM) {
            type[i] = 0;
            continue;
        }
        nbits = code2int[tp_code[i]][1];

        if (nbits >= 1)
            diff = BIT_readBitsFast(&transCodeStream, nbits);
        else
            diff =0;
        BIT_reloadDStream(&transCodeStream);
        
        if (tp_code[i] < POSITIVE_CODE_NUM) {
            // positve
            type[i] = code2type[tp_code[i]] + diff;
        }
        else{
            // negtive
            type[i] = code2type[tp_code[i]] - diff;
        }
		// printf(" %d:factor=%d, base=%d, nbits=%d, tp_code=%d, diff=%d\n", i, factor, base, nbits, tp_code[i], diff);
    }
    
    free(tp_code);
#ifdef TIMER__
    time_cost_end();
    printf("[adt-fse-2]: transcode time=%f\n", timeCost);
#endif

}

