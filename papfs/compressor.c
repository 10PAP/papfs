#include "compressor.h"
#include "log.h"

#define OFFSET_PTR_SIZE 4
static WaveletNode * load_node(FILE * file) {
    WaveletNode * current_node = (WaveletNode *) malloc(sizeof(WaveletNode));
    current_node->left = NULL;
    current_node->right = NULL;

    // read bitmap offset
    uint32_t bitmap_off;
    fread(&bitmap_off, OFFSET_PTR_SIZE, 1, file);

    // read bitmap
    off_t temp = ftell(file);
    fseek(file, bitmap_off, SEEK_SET);
    BIT_ARRAY * bitmap = bit_array_create(1);
    bit_array_load(bitmap, file, 4);
    fseek(file, temp, SEEK_SET);

    // fill node
    current_node->bitmap = bitmap;

    uint8_t children = 0;
    fread(&children, 1, 1, file);

    if ((children & 0b00000001) != 0) {
        current_node->left = load_node(file);
    }
    if ((children & 0b00000010) != 0) {
        current_node->right = load_node(file);
    }

    //DEBUG: log node's content
    log_print("NODE: \n");
    int len = barlen(current_node->bitmap);
    log_print("len: %d\n", len);
    //char *str = (char *) malloc(len + 1);
    //bit_array_to_str(current_node->bitmap, str);
    //log_print("Bitmap: %s\n", str);
    log_print("Left child %p\n", current_node->left);
    log_print("Right child %p\n", current_node->right);

    return current_node;
}

int load_metadata(int fd) {

    FILE *file = fdopen(fd, "rb");
    if (!file) {
        log_error("PAPFS_open error caused by fdopen\n");
    }

    //read huffman codes
    for (int i = 0; i < ALPHABETSIZE; i++) {

        BIT_ARRAY * barr = barcreate(1);
        char load_ret = barload(barr, file, 1);
        if (load_ret == 0) {
            log_error("Error in huffman code loading ");
        }

        PAPFS_DATA->huffCodes[i] = barr;
    }

    log_print("DEBUG: huffcodes reading complete");

    //DEBUG: log huffman codes
    /*for (int j = 0; j < ALPHABETSIZE; j++) {
        int len = barlen(PAPFS_DATA->huffCodes[j]);
        char *str = (char *) malloc(len + 1);
        bit_array_to_str(PAPFS_DATA->huffCodes[j], str);
        log_print("huff[%d]: %s\n", j, str);
        free(str);
    }*/

    // Wavelet-tree reading
    WaveletNode * root = load_node(file);
    PAPFS_DATA->wavelet_root = root;

    fclose(file);
    return 0;
}

void ra_write() {

}

void ra_read() {

}