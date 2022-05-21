all : papfs.o log.o fs_opers.o compressor.o bit_array.o
	gcc -g -o papfs papfs.o log.o fs_opers.o compressor.o bit_array.o `pkg-config fuse --libs`

papfs.o : papfs.c log.h params.h fs_opers.h compressor.h
	gcc -g -Wall `pkg-config fuse --cflags` -c papfs.c

log.o : log.c log.h params.h
	gcc -g -Wall `pkg-config fuse --cflags` -c log.c

fs_opers.o : fs_opers.c fs_opers.h params.h
	gcc -g -Wall `pkg-config fuse --cflags` -c fs_opers.c

compressor.o : compressor.c compressor.h params.h
	gcc -g -Wall `pkg-config fuse --cflags` -c compressor.c

bit_array.o : bitarray/bit_array.c bitarray/bit_array.h
	gcc -g -Wall `pkg-config fuse --cflags` -c bitarray/bit_array.c

clean:
	rm -f papfs *.o test

test:
	rm -rf test/
	mkdir test/
	mkdir test/root
	mkdir test/mount
	cp papfs test/papfs
	cp data.bin test/root/data.bin

test.o : test.c
	gcc -g -DTEST -Wall `pkg-config fuse --cflags` -c test.c

test_cases : log_d.o fs_opers_d.o compressor_d.o bit_array_d.o test.o
	gcc -g -DTEST -o test test.o log_d.o fs_opers_d.o compressor_d.o bit_array_d.o `pkg-config fuse --libs` -lcunit

log_d.o : log.c log.h params.h
	gcc -g -o log_d.o -DTEST -Wall `pkg-config fuse --cflags` -c log.c

fs_opers_d.o : fs_opers.c fs_opers.h params.h
	gcc -g -o fs_opers_d.o -DTEST -Wall `pkg-config fuse --cflags` -c fs_opers.c

compressor_d.o : compressor.c compressor.h params.h
	gcc -g -o compressor_d.o -DTEST -Wall `pkg-config fuse --cflags` -c compressor.c

bit_array_d.o : bitarray/bit_array.c bitarray/bit_array.h
	gcc -g -o bit_array_d.o -DTEST -Wall `pkg-config fuse --cflags` -c bitarray/bit_array.c