#!/bin/bash

SZ_PATH=/home/zhongyu/sz/sz2
TEST_FILE=/home/zhongyu/test/SDRBENCH-EXAALT-2869440-f/vx.dat2
ERROR_BOUND_MODE=ABS
PPP=A
ERROR_BOUND=1E-1
DIM=1
# 
# 10M
filesize_set_rows="2869440"
# for filesize_set_rows in $filesize_set_rows
# do
echo $filesize_set_rows

# change path
# ./configure --prefix=$SZ_PATH

make -j8 >/dev/null   

sudo make install >/dev/null

# output=/home/zhongyu/tmp/result.txt

# echo "sz -z -f -g -i $TEST_FILE -M $ERROR_BOUND_MODE -$PPP $ERROR_BOUND -1 $filesize_set_rows"
# echo "huffman:"
# echo "---compress---"
# # echo "sz -z -f -g 0 -i $TEST_FILE -M $ERROR_BOUND_MODE -$PPP $ERROR_BOUND -$DIM $filesize_set_rows"
# $SZ_PATH"/bin/"sz -z -f -g 0 -i $TEST_FILE -M $ERROR_BOUND_MODE -$PPP $ERROR_BOUND -$DIM $filesize_set_rows #> $output
# filesize_compressed=`ls -l $TEST_FILE".sz" | awk '{print $5}'`
# echo "$filesize_compressed"

# echo "---decompress---"
# # # echo "$SZ_PATH"/bin/"sz -x -f -s $TEST_FILE.sz -1 $filesize_set_rows"
# $SZ_PATH"/bin/"sz -x -f -s $TEST_FILE.sz -$DIM $filesize_set_rows #>> $output

# echo "zstd:"
# echo "---compress---"
# # echo "$SZ_PATH"/bin/"sz -z -f -g 1 -i $TEST_FILE -M $ERROR_BOUND_MODE -$PPP $ERROR_BOUND -$DIM $filesize_set_rows"
# $SZ_PATH"/bin/"sz -z -f -g 1 -i $TEST_FILE -M $ERROR_BOUND_MODE -$PPP $ERROR_BOUND -$DIM $filesize_set_rows #> $output
# filesize_compressed=`ls -l $TEST_FILE".sz" | awk '{print $5}'`
# echo "$filesize_compressed"

# echo "--decompress---"
# # echo "$SZ_PATH"/bin/"sz -x -f -s $TEST_FILE.sz -$DIM $filesize_set_rows"
# $SZ_PATH"/bin/"sz -x -f -s $TEST_FILE.sz -$DIM $filesize_set_rows #>> $output

echo "fse:"
echo "---compress---"
echo "$SZ_PATH"/bin/"sz -z -f -g 0 -i $TEST_FILE -M $ERROR_BOUND_MODE -$PPP $ERROR_BOUND -$DIM $filesize_set_rows"
$SZ_PATH"/bin/"sz -z -f -g 2 -i $TEST_FILE -M $ERROR_BOUND_MODE -$PPP $ERROR_BOUND -$DIM $filesize_set_rows #> $output
filesize_compressed=`ls -l $TEST_FILE".sz" | awk '{print $5}'`
filesize_origin=`ls -l $TEST_FILE | awk '{print $5}'`
echo "$filesize_compressed"
echo `echo "scale=6;$filesize_origin/$filesize_compressed" | bc`

echo "--decompress---"
echo "$SZ_PATH"/bin/"sz -x -f -s $TEST_FILE.sz -$DIM $filesize_set_rows"
$SZ_PATH"/bin/"sz -x -f -s $TEST_FILE.sz -$DIM $filesize_set_rows #>> $output

# rm -rf $TEST_FILE.sz $TEST_FILE.sz.out

# /bin/python3 /home/zhongyu/tmp/figure.py
# done

