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

int random_access_read_symbol(int fd, unsigned long i);
void ra_read();

#endif
