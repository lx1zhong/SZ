/**
 *  @file TightDataPointStorageD.h
 *  @author Sheng Di
 *  @date April, 2016
 *  @brief Header file for the tight data point storage (TDPS).
 *  (C) 2016 by Mathematics and Computer Science (MCS), Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#ifndef _TightDataPointStorageD_H
#define _TightDataPointStorageD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "zstd/common/bitstream.h"

typedef struct TightDataPointStorageD
{
	int entropyType;
	size_t dataSeriesLength;
	int allSameData;
	double realPrecision;
	double medianValue;
	char reqLength;	
	char radExpo; //used to compute reqLength based on segmented precisions in "pw_rel_compression"

	double minLogValue;

	int stateNum;
	int allNodes;

	size_t exactDataNum;
	double reservedValue;
	
	unsigned char* rtypeArray;
	size_t rtypeArray_size;

	unsigned char* FseCode; // fse code of tp_code
	size_t FseCode_size;

	unsigned char* transCodeBits; // extra bitstream of transcoding
	size_t transCodeBits_size;
	
	unsigned char* typeArray; //its size is dataSeriesLength/4 (or xxx/4+1) 
	size_t typeArray_size;
	
	unsigned char* leadNumArray; //its size is exactDataNum/4 (or exactDataNum/4+1)
	size_t leadNumArray_size;
	
	unsigned char* exactMidBytes;
	size_t exactMidBytes_size;
	
	unsigned char* residualMidBits;
	size_t residualMidBits_size;
	
	unsigned int intervals;
	
	unsigned char isLossless; //a mark to denote whether it's lossless compression (1 is yes, 0 is no)
	
	size_t segment_size;
	
	unsigned char* pwrErrBoundBytes;
	int pwrErrBoundBytes_size;
		
	unsigned char* raBytes;
	size_t raBytes_size;
	
	unsigned char plus_bits;
	unsigned char max_bits;
	
} TightDataPointStorageD;

void new_TightDataPointStorageD_Empty(TightDataPointStorageD **self);
int new_TightDataPointStorageD_fromFlatBytes(TightDataPointStorageD **self, unsigned char* flatBytes, size_t flatBytesLength);

void new_TightDataPointStorageD(TightDataPointStorageD **self, 
		size_t dataSeriesLength, size_t exactDataNum, 
		int* type, unsigned char* exactMidBytes, size_t exactMidBytes_size,
		unsigned char* leadNumIntArray,  //leadNumIntArray contains readable numbers....
		unsigned char* resiMidBits, size_t resiMidBits_size,
		unsigned char resiBitLength, 
		double realPrecision, double medianValue, char reqLength, unsigned int intervals, 
		unsigned char* pwrErrBoundBytes, size_t pwrErrBoundBytes_size, unsigned char radExpo);

void new_TightDataPointStorageD2(TightDataPointStorageD **self, 
		size_t dataSeriesLength, size_t exactDataNum, 
		int* type, unsigned char* exactMidBytes, size_t exactMidBytes_size,
		unsigned char* leadNumIntArray,  //leadNumIntArray contains readable numbers....
		unsigned char* resiMidBits, size_t resiMidBits_size,
		unsigned char* resiBitLength, size_t resiBitLengthSize,
		double realPrecision, double medianValue, char reqLength, unsigned int intervals,
		unsigned char* pwrErrBoundBytes, size_t pwrErrBoundBytes_size, unsigned char radExpo);

void convertTDPStoBytes_double(TightDataPointStorageD* tdps, unsigned char* bytes, unsigned char* dsLengthBytes, unsigned char sameByte);
void convertTDPStoBytes_double_reserve(TightDataPointStorageD* tdps, unsigned char* bytes, unsigned char* dsLengthBytes, unsigned char sameByte);
void convertTDPStoFlatBytes_double(TightDataPointStorageD *tdps, unsigned char** bytes, size_t *size);
void convertTDPStoFlatBytes_double_args(TightDataPointStorageD *tdps, unsigned char* bytes, size_t *size);

void free_TightDataPointStorageD(TightDataPointStorageD *tdps);
void free_TightDataPointStorageD2(TightDataPointStorageD *tdps);

void printTDPSD(TightDataPointStorageD *tdps);

#ifdef __cplusplus
}
#endif

#endif /* ----- #ifndef _TightDataPointStorageD_H  ----- */
