all : papfs.o log.o fs_opers.o compressor.o bit_array.o libcversion.a
	gcc -g -O0 -fsanitize=address -static-libasan -o papfs papfs.o log.o fs_opers.o compressor.o bit_array.o `pkg-config fuse --libs` -L. -lcversion

papfs.o : papfs.c log.h params.h fs_opers.h compressor.h
	gcc -g -Wall -fsanitize=address `pkg-config fuse --cflags` -c papfs.c 

log.o : log.c log.h params.h
	gcc -g -Wall -fsanitize=address `pkg-config fuse --cflags` -c log.c

fs_opers.o : fs_opers.c fs_opers.h params.h
	gcc -g -Wall -fsanitize=address `pkg-config fuse --cflags` -c fs_opers.c

compressor.o : compressor.c compressor.h params.h
	gcc -g -O0 -Wall -fsanitize=address `pkg-config fuse --cflags` -c compressor.c

bit_array.o : bitarray/bit_array.c bitarray/bit_array.h
	gcc -g -Wall -fsanitize=address `pkg-config fuse --cflags` -c bitarray/bit_array.c

huffman.o : cversion/huffman.c cversion/huffman.h
	gcc -O -c cversion/huffman.c -o huffman.o

main.o : cversion/main.c cversion/cversion.h
	gcc -O -c cversion/main.c -o main.o

libcversion.a : bit_array.o huffman.o main.o
	ar rcs libcversion.a bit_array.o huffman.o main.o

clean:
	rm -f papfs *.o test *.a

test:
	rm -rf test/
	mkdir test/
	mkdir test/root
	mkdir test/mount
	cp papfs test/papfs
	cp data.bin test/root/data.bin

test.o : test.c
	gcc -g -DTEST -Wall -fsanitize=address -static-libasan `pkg-config fuse --cflags` -c test.c

test_cases : log_d.o fs_opers_d.o compressor_d.o bit_array_d.o test.o
	gcc -g -fsanitize=address -static-libasan -DTEST -o test test.o log_d.o fs_opers_d.o compressor_d.o bit_array_d.o `pkg-config fuse --libs` -lcunit

log_d.o : log.c log.h params.h
	gcc -g -fsanitize=address -static-libasan -o log_d.o -DTEST -Wall `pkg-config fuse --cflags` -c log.c

fs_opers_d.o : fs_opers.c fs_opers.h params.h
	gcc -g -fsanitize=address -static-libasan -o fs_opers_d.o -DTEST -Wall `pkg-config fuse --cflags` -c fs_opers.c

compressor_d.o : compressor.c compressor.h params.h
	gcc -g -fsanitize=address -static-libasan -o compressor_d.o -DTEST -Wall `pkg-config fuse --cflags` -c compressor.c

bit_array_d.o : bitarray/bit_array.c bitarray/bit_array.h
	gcc -g -fsanitize=address -static-libasan -o bit_array_d.o -DTEST -Wall `pkg-config fuse --cflags` -c bitarray/bit_array.c
