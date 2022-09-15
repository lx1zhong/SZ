#!/bin/bash

SZ_PATH=/home/lxzhong/sz/sz1/bin
TEST_FILE=/home/lxzhong/test/SDRBENCH-EXASKY_NYX-512x512x512-f/baryon_density.f32
ERROR_BOUND_MODE=PW_REL
ERROR_BOUND=1E-5
DIM=3

# 10M
filesize_set_rows="512 512 512"
# for filesize_set_rows in $filesize_set_rows
# do
echo $filesize_set_rows

make -j8

sudo make install 

# output=/home/lxzhong/tmp/result.txt

# echo "sz -z -f -e -i $TEST_FILE -M $ERROR_BOUND_MODE -P $ERROR_BOUND -1 $filesize_set_rows"
echo "huffman:"
echo "---compress---"
# echo "sz -z -f -e 0 -i $TEST_FILE -M $ERROR_BOUND_MODE -P $ERROR_BOUND -1 $filesize_set_rows"
$SZ_PATH"/"sz -z -f -e 0 -i $TEST_FILE -M $ERROR_BOUND_MODE -P $ERROR_BOUND -$DIM $filesize_set_rows #> $output
filesize_compressed=`ls -l $TEST_FILE".sz" | awk '{print $5}'`
echo "$filesize_compressed"

echo "---decompress---"
# echo "$SZ_PATH"/"sz -x -f -s $TEST_FILE.sz -1 $filesize_set_rows"
$SZ_PATH"/"sz -x -f -s $TEST_FILE.sz -$DIM $filesize_set_rows #>> $output

# echo "zstd:"
# echo "---compress---"
# echo "$SZ_PATH"/"sz -z -f -e 1 -i $TEST_FILE -M $ERROR_BOUND_MODE -P $ERROR_BOUND -$DIM $filesize_set_rows"
# $SZ_PATH"/"sz -z -f -e 1 -i $TEST_FILE -M $ERROR_BOUND_MODE -P $ERROR_BOUND -$DIM $filesize_set_rows #> $output
# filesize_compressed=`ls -l $TEST_FILE".sz" | awk '{print $5}'`
# echo "$filesize_compressed"

# echo "--decompress---"
# echo "$SZ_PATH"/"sz -x -f -s $TEST_FILE.sz -$DIM $filesize_set_rows"
# $SZ_PATH"/"sz -x -f -s $TEST_FILE.sz -$DIM $filesize_set_rows #>> $output

echo "fse:"
echo "---compress---"
echo "$SZ_PATH"/"sz -z -f -e 2 -i $TEST_FILE -M $ERROR_BOUND_MODE -P $ERROR_BOUND -$DIM $filesize_set_rows"
$SZ_PATH"/"sz -z -f -e 2 -i $TEST_FILE -M $ERROR_BOUND_MODE -P $ERROR_BOUND -$DIM $filesize_set_rows #> $output
filesize_compressed=`ls -l $TEST_FILE".sz" | awk '{print $5}'`
echo "$filesize_compressed"

echo "--decompress---"
# echo "$SZ_PATH"/"sz -x -f -s $TEST_FILE.sz -1 $filesize_set_rows"
$SZ_PATH"/"sz -x -f -s $TEST_FILE.sz -$DIM $filesize_set_rows #>> $output

rm -rf $TEST_FILE.sz $TEST_FILE.sz.out

# /bin/python3 /home/lxzhong/tmp/figure.py
# done

