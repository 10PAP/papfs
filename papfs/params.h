#ifndef _PARAMS_H_
#define _PARAMS_H_

#include <limits.h>
#include <stdio.h>
#include "bitarray/bit_array.h"

#define FUSE_USE_VERSION 26

#define _XOPEN_SOURCE 500

#define ALPHABETSIZE (256)


struct fs_state {
    FILE *logfile;
    char *rootdir;
    // and some data for compressor
    BIT_ARRAY * huffCodes[ALPHABETSIZE];
};

#define PAPFS_DATA ((struct fs_state *) fuse_get_context()->private_data)

#include <fuse.h>

#endif
