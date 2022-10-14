## * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
##   Makefile to test SZ library.

##=======================================================================
##   PLEASE SET THESE VARIABLES BEFORE COMPILING
##=======================================================================

SZPATH		= /home/zhongyu/sz/sz2
HDF5PATH	= /usr/lib/x86_64-linux-gnu/hdf5/serial

##   COMPILERS
CC		= gcc
MPICC 		= mpicc
MPIRUN 		= mpirun

##=======================================================================
##   FLAGS
##=======================================================================

#SZFLAGS		= -I$(SZPATH)/include $(SZPATH)/lib/libSZ.a $(SZPATH)/lib/libzlib.a $(SZPATH)/lib/libhdf5sz.a
SZFLAGS		= -I$(SZPATH)/include -I/home/zhongyu/sz/SZ/hdf5-filter/H5Z-SZ/include -I/home/zhongyu/sz/SZ -L$(SZPATH)/lib -Wl,-rpath,$(SZPATH)/lib 

#HDF5FLAGS	= -I$(HDF5PATH)/include $(HDF5PATH)/lib/libhdf5.a
HDF5FLAGS	= -I$(HDF5PATH)/include -L$(HDF5PATH)/lib -Wl,-rpath,$(HDF5PATH)/lib -L/home/zhongyu/sz/SZ/hdf5-filter/H5Z-SZ/lib

##=======================================================================
##   TARGETS
##=======================================================================
all: 		szToHDF5 dszFromHDF5 convertBinToHDF5 #test_mpio

szToHDF5:	szToHDF5.c
	$(MPICC)  -O3 -o szToHDF5 szToHDF5.c $(SZFLAGS) $(HDF5FLAGS) -lSZ -lzlib -lhdf5sz -lhdf5 -lm -Wl,--no-as-needed -ldl

dszFromHDF5:	dszFromHDF5.c
	$(MPICC)  -O3 -o dszFromHDF5 dszFromHDF5.c $(SZFLAGS) $(HDF5FLAGS) -lSZ -lzlib -lhdf5sz -lhdf5 -lm -Wl,--no-as-needed -ldl

convertBinToHDF5:	convertBinToHDF5.c
	$(MPICC)  -O3 -o convertBinToHDF5 convertBinToHDF5.c $(SZFLAGS) $(HDF5FLAGS) -lSZ -lzlib -lhdf5sz -lhdf5 -lm -Wl,--no-as-needed -ldl

test_mpio:	test_mpio.c
	$(MPICC)  -O3 -o test_mpio test_mpio.c $(SZFLAGS) $(HDF5FLAGS) -lSZ -lzlib -lhdf5sz -lhdf5 -lm -Wl,--no-as-needed -ldl

clean:
	rm -f szToHDF5 dszFromHDF5 convertBinToHDF5 test_mpio

