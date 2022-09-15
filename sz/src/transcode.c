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
    // int max_type=0, min_type=0;
    // int diff;

    // int *type_ = (int*)malloc(dataSeriesLength*sizeof(int));
    // memcpy(type_, type, dataSeriesLength*sizeof(int));

    // qsort(type_, dataSeriesLength, sizeof(int), comp);
    // int first=0;
    // while(type_[first]==0) first++;
    // printf("【type】max=%d, mid=%d, intervals/2=%d\n", type_[dataSeriesLength-1], type_[(dataSeriesLength-first)/2 + first], intervals/2);

    // transcoding results of type array
    uint8_t *tp_code = (uint8_t *)malloc(dataSeriesLength);
    // uint8_t tp_code[dataSeriesLength];           // 数值太大时，初始化数组会失败(why?)
    int nbits;

    BIT_CStream_t transCodeStream;
    int dstCapacity = dataSeriesLength * sizeof(int);
    void *tmp = malloc(dstCapacity);
    BIT_initCStream(&transCodeStream, tmp, dstCapacity);

#ifdef TIMER__
	huff_cost_start3();
#endif
    // transcoding
    // printf("intervals=%u\n", intervals);
    int md = intervals/2;

    // 提前初始化数组，尽量精简长循环内的代码
    uint8_t type2code[intervals];
    unsigned int diff[intervals];
    for (int i=md; i<intervals; i++) {
        type2code[i] = (uint8_t)Int2code(i-md);
        diff[i] = i - md - code2int[type2code[i]][0];
    }
    for (int i=md-1; i>0; i--) {
        type2code[i] = 67-type2code[2*md-i];
        diff[i] = diff[2*md-i];
    }

#ifdef TIMER__
    huff_cost_end3();
    printf("[fse-0]: table time=%f\n", huffCost3);

	huff_cost_start3();
#endif

    for (int i=0; i<dataSeriesLength; i++) {    // 0.677
        // if (type[i] > max_type)
        // 	max_type = type[i];
        // if (type[i] < min_type)
        // 	min_type = type[i];
        if (type[i] == 0) {
            // unpredictable data
            tp_code[i] = 67;
            nbits = 0;
        }
        else {
            tp_code[i] = type2code[type[i]];                                                                      //0.45
            nbits = code2int[tp_code[i]][1];
            BIT_addBitsFast(&transCodeStream, diff[type[i]], nbits);                      //0.03
            // printf(" %d: type=%d, nbits=%d, tp_code=%d, diff=%d\n", i, type[i], nbits, tp_code[i], diff[type[i]]);
            BIT_flushBitsFast(&transCodeStream);                                                                 //0.1
        }
    }
    
#ifdef TIMER__
    huff_cost_end3();
    printf("[fse-1]: transcode time=%f\n", huffCost3);
    // fse encoding
    
	huff_cost_start3();
#endif

    (*FseCode) = (unsigned char*)malloc(dataSeriesLength);
    size_t fse_size = FSE_compress((*FseCode), dataSeriesLength, tp_code, dataSeriesLength);
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
    (*transCodeBits) = malloc(streamSize);
    memcpy((*transCodeBits), tmp, streamSize);
    free(tmp);
    
#ifdef TIMER__
    huff_cost_end3();
    printf("[fse-2]: encode time=%f\n", huffCost3);
#endif

    // FILE *f0 = fopen("/home/lxzhong/tmp/type_array.bin","wb");
    // fwrite(type, sizeof(int), dataSeriesLength, f0);
    // fclose(f0);

    // FILE *f1 = fopen("/home/lxzhong/tmp/tp_code.txt","w");
    // for (int j=0; j<dataSeriesLength; j++) {
    // 	fprintf(f1, "%d ", (int)tp_code[j]);
    // }
    // fclose(f1);

    // FILE *f2 = fopen("/home/lxzhong/tmp/fse.bin","wb");
    // fwrite((*FseCode), 1, fse_size, f2);
    // fclose(f2);

    // FILE *f3 = fopen("/home/lxzhong/tmp/transCodeBits.bin","wb");
    // fwrite((*transCodeBits), 1, streamSize, f3);
    // fclose(f3);

    // int* type2 = (int*)malloc(dataSeriesLength*sizeof(int));
    // decode_with_fse((*this), dataSeriesLength, type2);
    // printf("max_type=%d, min_type=%d\n", max_type, min_type);

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
    // printf("intervals = %u\n", intervals);

    int code2type[67];
    for (int i=0; i<67; i++) {
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
        if (tp_code[i] == 67) {
            type[i] = 0;
            continue;
        }
        nbits = code2int[tp_code[i]][1];

        if (nbits >= 1)
            diff = BIT_readBitsFast(&transCodeStream, nbits);
        else
            diff =0;
        BIT_reloadDStream(&transCodeStream);
        
        if (tp_code[i] < 34) {
            // 正数
            type[i] = code2type[tp_code[i]] + diff;
        }
        else{
            // 负数
            type[i] = code2type[tp_code[i]] - diff;
        }
		// printf(" %d:factor=%d, base=%d, nbits=%d, tp_code=%d, diff=%d\n", i, factor, base, nbits, tp_code[i], diff);
    }
    
#ifdef TIMER__
    huff_cost_end3();
    printf("[fse-3]: transcode time=%f\n", huffCost3);
#endif
    
    // FILE *f0 = fopen("/home/lxzhong/tmp/type_array2.bin","wb");
    // fwrite(type, sizeof(int), dataSeriesLength, f0);
    // fclose(f0);

}

