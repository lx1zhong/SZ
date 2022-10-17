#!/bin/bash

echo "Set \$YOUR_INSTALL_PATH if you use SZ at the first time!"
YOUR_INSTALL_PATH=#set this

# set sz path
SZ_PATH=$YOUR_INSTALL_PATH/bin/sz

# set test file
# for fuctional test only, for more test, you may download SDRBENCH dataset from here: https://sdrbench.github.io/
TEST_FILE=./example/testdata/x86/testfloat_8_8_128.dat
DIM=3
dims="128 8 8"

# set error bound
ERROR_BOUND_MODE=ABS
PREFIX=A
ERROR_BOUND=1E-1

# configuration
# only need to open this when first time you install sz
./configure --prefix=$YOUR_INSTALL_PATH

# make
make -j8 #>/dev/null   

# installation
sudo make install #>/dev/null

# run
echo "sz -z -f -g <n> -i $TEST_FILE -M $ERROR_BOUND_MODE -$PREFIX $ERROR_BOUND -$DIM $dims"

echo
echo "Huffman:"
echo "---compress---"
# echo "sz -z -f -g 0 -i $TEST_FILE -M $ERROR_BOUND_MODE -$PREFIX $ERROR_BOUND -$DIM $dims"
$SZ_PATH -z -f -g 0 -i $TEST_FILE -M $ERROR_BOUND_MODE -$PREFIX $ERROR_BOUND -$DIM $dims
filesize_compressed=`ls -l $TEST_FILE".sz" | awk '{print $5}'`
filesize_origin=`ls -l $TEST_FILE | awk '{print $5}'`
echo compressed bytes: "$filesize_compressed"
echo compression ratio: `echo "scale=6;$filesize_origin/$filesize_compressed" | bc`

echo "---decompress---"
# # echo "$SZ_PATH -x -f -s $TEST_FILE.sz -1 $dims"
$SZ_PATH -x -f -s $TEST_FILE.sz -$DIM $dims #>> $output

echo
echo "Zstd:"
echo "---compress---"
# echo "$SZ_PATH -z -f -g 1 -i $TEST_FILE -M $ERROR_BOUND_MODE -$PREFIX $ERROR_BOUND -$DIM $dims"
$SZ_PATH -z -f -g 1 -i $TEST_FILE -M $ERROR_BOUND_MODE -$PREFIX $ERROR_BOUND -$DIM $dims
filesize_compressed=`ls -l $TEST_FILE".sz" | awk '{print $5}'`
filesize_origin=`ls -l $TEST_FILE | awk '{print $5}'`
echo compressed bytes: "$filesize_compressed"
echo compression ratio: `echo "scale=6;$filesize_origin/$filesize_compressed" | bc`

echo "--decompress---"
# echo "$SZ_PATH -x -f -s $TEST_FILE.sz -$DIM $dims"
$SZ_PATH -x -f -s $TEST_FILE.sz -$DIM $dims #>> $output

echo
echo "ADT-FSE:"
echo "---compress---"
# echo "$SZ_PATH -z -f -g 2 -i $TEST_FILE -M $ERROR_BOUND_MODE -$PREFIX $ERROR_BOUND -$DIM $dims"
$SZ_PATH -z -f -g 2 -i $TEST_FILE -M $ERROR_BOUND_MODE -$PREFIX $ERROR_BOUND -$DIM $dims
filesize_compressed=`ls -l $TEST_FILE".sz" | awk '{print $5}'`
filesize_origin=`ls -l $TEST_FILE | awk '{print $5}'`
echo compressed bytes: "$filesize_compressed"
echo compression ratio: `echo "scale=6;$filesize_origin/$filesize_compressed" | bc`

echo "--decompress---"
# echo "$SZ_PATH -x -f -s $TEST_FILE.sz -$DIM $dims"
$SZ_PATH -x -f -s $TEST_FILE.sz -$DIM $dims #>> $output

# rm -rf $TEST_FILE.sz $TEST_FILE.sz.out

