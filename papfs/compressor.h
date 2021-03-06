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

int random_access_read_symbol(int id, unsigned long i);
uint32_t getTreeRank(WaveletNode * node, unsigned long i, BIT_ARRAY * code, unsigned long current_level);
void ra_read();
unsigned int rank(char bit, unsigned int i, BIT_ARRAY * bitmap);
int getHuffmanCode(WaveletNode * node, unsigned long i, BIT_ARRAY * resBits, int l);
uint8_t decodeHuffmanCode(int id, BIT_ARRAY * bits);

#endif
