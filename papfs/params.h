#ifndef _PARAMS_H_
#define _PARAMS_H_

#include <limits.h>
#include <stdio.h>
#include "bitarray/bit_array.h"

#define FUSE_USE_VERSION 26

#define _XOPEN_SOURCE 500

#define ALPHABETSIZE (256)

#define MAX_N_OF_FILES 1024

typedef struct WaveletNode {
    BIT_ARRAY * bitmap;
    struct WaveletNode * left;
    struct WaveletNode * right;
} WaveletNode;

typedef struct File_Metadata {
    BIT_ARRAY * huffCodes[ALPHABETSIZE];
    WaveletNode * wavelet_root;
} File_Metadata;

struct fs_state {
    FILE *logfile;
    char *rootdir;
    // and some data for compressor
    File_Metadata metadata[MAX_N_OF_FILES];
    int fd_table[MAX_N_OF_FILES];
    int opened_N;
};

int fd_to_id(int fd);

#ifndef TEST
#define PAPFS_DATA ((struct fs_state *) fuse_get_context()->private_data)
#else
struct fs_state * temp_state;
#define PAPFS_DATA temp_state
#endif

#include <fuse.h>

#endif
