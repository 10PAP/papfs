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
	gcc -g -Wall `pkg-config fuse --cflags` -c test.c

test_cases : log.o fs_opers.o compressor.o bit_array.o test.o
	gcc -g -o test test.o log.o fs_opers.o compressor.o bit_array.o `pkg-config fuse --libs` -lcunit