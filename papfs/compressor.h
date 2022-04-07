#ifndef _COMPRESSOR_H_
#define _COMPRESSOR_H_

#include <sys/types.h>
#include <unistd.h>
#include <malloc.h>
#include <stdio.h>

#include "params.h"
#include "bitarray/bit_array.h"
#include "bitarray/bar.h"

int load_metadata(int fd);

void ra_write();
void ra_read();

#endif
