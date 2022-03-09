all : papfs.o log.o fs_opers.o compressor.o
	gcc -g -o papfs papfs.o log.o fs_opers.o compressor.o `pkg-config fuse --libs`

papfs.o : papfs.c log.h params.h fs_opers.h compressor.h
	gcc -g -Wall `pkg-config fuse --cflags` -c papfs.c

log.o : log.c log.h params.h
	gcc -g -Wall `pkg-config fuse --cflags` -c log.c

fs_opers.o : fs_opers.c fs_opers.h params.h
	gcc -g -Wall `pkg-config fuse --cflags` -c fs_opers.c

compressor.o : compressor.c compressor.h params.h
	gcc -g -Wall `pkg-config fuse --cflags` -c compressor.c

clean:
	rm -f papfs *.o

test:
	rm -rf test/
	mkdir test/
	mkdir test/root
	mkdir test/mount
	echo "some test file" > test/root/test.txt
	cp papfs test/papfs
