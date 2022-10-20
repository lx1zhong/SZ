#!/bin/bash
FILE=/media/hdd/vy.dat2
DIMS="2869440"

sudo rm -rf $FILE.sz.h5

export HDF5_PLUGIN_PATH=/home/zhongyu/sz/sz2/lib
export LD_LIBRARY_PATH=/home/zhongyu/sz/sz2/lib:/usr/lib/x86_64-linux-gnu/hdf5/serial/lib:$LD_LIBRARY_PATH

echo "【float】"
echo "===huffman==="
echo "  ==compress=="
# echo ./szToHDF5 -f sz.config ../../../example/testdata/x86/testfloat_8_8_128.dat 8 8 128
sudo echo 1 > /proc/sys/vm/drop_caches
./szToHDF5 -f sz.config $FILE $DIMS
filesize_compressed=`ls -l $FILE.sz.h5 | awk '{print $5}'`
filesize_origin=`ls -l $FILE | awk '{print $5}'`
echo "$filesize_compressed"
echo `echo "scale=6;$filesize_origin/$filesize_compressed" | bc`
echo "  ==decompress=="
# echo ./dszFromHDF5 ../../../example/testdata/x86/testfloat_8_8_128.dat.sz.h5
sudo echo 1 > /proc/sys/vm/drop_caches
./dszFromHDF5 $FILE.sz.h5

echo "===fse==="
echo "  ==compress=="
# echo ./szToHDF5 -f sz2.config ../../../example/testdata/x86/testfloat_8_8_128.dat 8 8 128
./szToHDF5 -f sz2.config $FILE $DIMS
filesize_compressed=`ls -l $FILE.sz.h5 | awk '{print $5}'`
filesize_origin=`ls -l $FILE | awk '{print $5}'`
echo "$filesize_compressed"
echo `echo "scale=6;$filesize_origin/$filesize_compressed" | bc`
sudo echo 1 > /proc/sys/vm/drop_caches
echo "  ==decompress=="
# echo ./dszFromHDF5 ../../../example/testdata/x86/testfloat_8_8_128.dat.sz.h5
./dszFromHDF5 $FILE.sz.h5

sudo rm -rf $FILE.sz.h5
# echo "【double】"
# echo "===huffman==="
# echo "  ==compress=="
# # echo ./szToHDF5 -d sz.config ../../../example/testdata/x86/testdouble_8_8_128.dat 8 8 128
# ./szToHDF5 -d sz.config ../../../example/testdata/x86/testdouble_8_8_128.dat 8 8 128
# filesize_compressed=`ls -l ../../../example/testdata/x86/testdouble_8_8_128.dat.sz.h5 | awk '{print $5}'`
# filesize_origin=`ls -l ../../../example/testdata/x86/testdouble_8_8_128.dat | awk '{print $5}'`
# echo "$filesize_compressed"
# echo `echo "scale=6;$filesize_origin/$filesize_compressed" | bc`
# echo "  ==decompress=="
# # echo ./dszFromHDF5 ../../../example/testdata/x86/testdouble_8_8_128.dat.sz.h5
# ./dszFromHDF5 ../../../example/testdata/x86/testdouble_8_8_128.dat.sz.h5

# echo "===fse==="
# echo "  ==compress=="
# # echo ./szToHDF5 -d sz2.config ../../../example/testdata/x86/testdouble_8_8_128.dat 8 8 128
# ./szToHDF5 -d sz2.config ../../../example/testdata/x86/testdouble_8_8_128.dat 8 8 128
# filesize_compressed=`ls -l ../../../example/testdata/x86/testdouble_8_8_128.dat.sz.h5 | awk '{print $5}'`
# filesize_origin=`ls -l ../../../example/testdata/x86/testdouble_8_8_128.dat | awk '{print $5}'`
# echo "$filesize_compressed"
# echo `echo "scale=6;$filesize_origin/$filesize_compressed" | bc`
# echo "  ==decompress=="
# # echo ./dszFromHDF5 ../../../example/testdata/x86/testdouble_8_8_128.dat.sz.h5
# ./dszFromHDF5 ../../../example/testdata/x86/testdouble_8_8_128.dat.sz.h5

# #echo ./szToHDF5 -i8 sz.config ../../../example/testdata/x86/testint8_8x8x8.dat 8 8 8
# #./szToHDF5 -i8 sz.config ../../../example/testdata/x86/testint8_8x8x8.dat 8 8 8
# #echo ./szToHDF5 -i16 sz.config ../../../example/testdata/x86/testint16_8x8x8.dat 8 8 8
# #./szToHDF5 -i16 sz.config ../../../example/testdata/x86/testint16_8x8x8.dat 8 8 8
# #echo ./szToHDF5 -i32 sz.config ../../../example/testdata/x86/testint32_8x8x8.dat 8 8 8
# #./szToHDF5 -i32 sz.config ../../../example/testdata/x86/testint32_8x8x8.dat 8 8 8
# #echo ./szToHDF5 -i64 sz.config ../../../example/testdata/x86/testint64_8x8x8.dat 8 8 8
# #./szToHDF5 -i64 sz.config ../../../example/testdata/x86/testint64_8x8x8.dat 8 8 8
# echo ./szToHDF5 -u8 sz.config ../../../example/testdata/x86/testint8_8x8x8.dat 8 8 8
# ./szToHDF5 -u8 sz.config ../../../example/testdata/x86/testint8_8x8x8.dat 8 8 8
# echo ./szToHDF5 -u16 sz.config ../../../example/testdata/x86/testint16_8x8x8.dat 8 8 8
# ./szToHDF5 -u16 sz.config ../../../example/testdata/x86/testint16_8x8x8.dat 8 8 8
# echo ./szToHDF5 -u32 sz.config ../../../example/testdata/x86/testint32_8x8x8.dat 8 8 8
# ./szToHDF5 -u32 sz.config ../../../example/testdata/x86/testint32_8x8x8.dat 8 8 8
# echo ./szToHDF5 -u64 sz.config ../../../example/testdata/x86/testint64_8x8x8.dat 8 8 8
# ./szToHDF5 -u64 sz.config ../../../example/testdata/x86/testint64_8x8x8.dat 8 8 8
