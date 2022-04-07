#include "compressor.h"
#include "log.h"

int load_metadata(int fd) {

    log_print("DEBUG: metadata\n");
    FILE *file = fdopen(fd, "rb");
    if (!file) {
        log_error("PAPFS_open error caused by fdopen\n");
    }
    log_print("DEBUG: metadata after fdopen\n");
    //read huffman codes
    for (int i = 0; i < ALPHABETSIZE; i++) {

        BIT_ARRAY * barr = barcreate(1);
        char load_ret = barload(barr, file, 1);
        if (load_ret == 0) {
            log_error("Error in huffman code loading ");
        }

        PAPFS_DATA->huffCodes[i] = barr;
    }

    fclose(file);

    log_print("DEBUG: huffcodes reading complete");

    //DEBUG
    for (int j = 0; j < ALPHABETSIZE; j++) {
        int len = barlen(PAPFS_DATA->huffCodes[j]);
        char *str = (char *) malloc(len + 1);
        bit_array_to_str(PAPFS_DATA->huffCodes[j], str);
        log_print("huff[%d]: %s\n", j, str);
        free(str);
    }
    return 0;
}

void ra_write() {

}

void ra_read() {

}