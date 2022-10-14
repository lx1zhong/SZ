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

struct timeval Start3; /*only used for recording the cost*/
double huffCost3 = 0;


void huff_cost_start3()
{
	huffCost3 = 0;
	gettimeofday(&Start3, NULL);
}

void huff_cost_end3()
{
	double elapsed;
	struct timeval costEnd;
	gettimeofday(&costEnd, NULL);
	elapsed = ((costEnd.tv_sec*1000000+costEnd.tv_usec)-(Start3.tv_sec*1000000+Start3.tv_usec))/1000000.0;
	huffCost3 += elapsed;
}
#endif

// int comp(const void *a, const void *b) {
//     return *(int *)a - *(int *)b;
// }

/**
 * @brief transform type array to FseCode & tranCodeBits
 * [type] ---minus md---> [factor] ---transcode---> [tp_code]+[bitstream of diff]
 * 
 */
void encode_with_fse(int *type, size_t dataSeriesLength, unsigned int intervals, 
                    unsigned char **FseCode, size_t *FseCode_size, 
                    unsigned char **transCodeBits, size_t *transCodeBits_size) {

    // int *type_ = (int*)malloc(dataSeriesLength*sizeof(int));
    // memcpy(type_, type, dataSeriesLength*sizeof(int));

    // qsort(type_, dataSeriesLength, sizeof(int), comp);
    // int first=0;
    // while(type_[first]==0) first++;
    // printf("【type】max=%d, mid=%d, intervals/2=%d\n", type_[dataSeriesLength-1], type_[(dataSeriesLength-first)/2 + first], intervals/2);

    // transcoding results of type array
    uint8_t *tp_code = (uint8_t *)malloc(dataSeriesLength);
    int nbits;

    BIT_CStream_t transCodeStream;
    int dstCapacity = dataSeriesLength * sizeof(int);
    (*transCodeBits) = (unsigned char *)malloc(dstCapacity);
    BIT_initCStream(&transCodeStream, (*transCodeBits), dstCapacity);

#ifdef TIMER__
	huff_cost_start3();
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
    huff_cost_end3();
    printf("[fse-0]: table time=%f\n", huffCost3);

	huff_cost_start3();
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
    huff_cost_end3();
    printf("[fse-1]: transcode time=%f\n", huffCost3);
    // fse encoding
    
	huff_cost_start3();
#endif

    (*FseCode) = (unsigned char*)malloc(2 * dataSeriesLength);
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
    huff_cost_end3();
    printf("[fse-2]: encode time=%f\n", huffCost3);
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
	huff_cost_start3();
#endif

    uint8_t *tp_code = (uint8_t *)malloc(dataSeriesLength);

    if (FseCode_size <= 1) {
        // all 0
        memset((void *)type, 0, sizeof(int) * dataSeriesLength);
        return;
    }
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
    huff_cost_end3();
    printf("[fse-1]: fse decode time=%f\n", huffCost3);
    
	huff_cost_start3();
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
    huff_cost_end3();
    printf("[fse-2]: table time=%f\n", huffCost3);
    
	huff_cost_start3();
#endif

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
    huff_cost_end3();
    printf("[fse-3]: transcode time=%f\n", huffCost3);
#endif

}

