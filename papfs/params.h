#ifndef _PARAMS_H_
#define _PARAMS_H_

#include <limits.h>
#include <stdio.h>
#include "bitarray/bit_array.h"

#define FUSE_USE_VERSION 26

#define _XOPEN_SOURCE 500

#define ALPHABETSIZE (256)

typedef struct WaveletNode {
    BIT_ARRAY * bitmap;
    struct WaveletNode * left;
    struct WaveletNode * right;
} WaveletNode;

//TODO: list of huffcodes and wavelet roots
// because we want to open several files simultaneously :)
struct fs_state {
    FILE *logfile;
    char *rootdir;
    // and some data for compressor
    BIT_ARRAY * huffCodes[ALPHABETSIZE];
    WaveletNode * wavelet_root;
};

#define PAPFS_DATA ((struct fs_state *) fuse_get_context()->private_data)

#include <fuse.h>

#endif
