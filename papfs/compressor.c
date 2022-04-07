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

unsigned int rank(char bit, unsigned int i, BIT_ARRAY * bitmap){
    unsigned int res = 0;
    for(int j = 0 ; j <= i ; j++)
        if(bit_array_get_bit(bitmap, j) == bit)
            res++;
    return res;
}

int getHuffmanCode(WaveletNode * node, unsigned long i, BIT_ARRAY * resBits, int l){
    //log_print("BITMAP LEN = %lu, i = %lu\n", barlen(node->bitmap), i);
    if (i >= barlen(node->bitmap)) {
        return -1;
    }
    char bit = bit_array_get_bit(node->bitmap, i);
    unsigned int position = rank(bit, i, node->bitmap)-1;
    WaveletNode * next = (!bit) ? node->left : node->right;

    if (bit == 0) {
        barclr(resBits, l-1);
    } else {
        barset(resBits, l-1);
    }

    if(next == NULL)
        return l;

    return getHuffmanCode(next, position, resBits, l+1);
}

uint8_t decodeHuffmanCode(BIT_ARRAY * bits){
    for(int i = 0 ; i < ALPHABETSIZE ; i++){
        char success = 1;
        BIT_ARRAY * code = PAPFS_DATA->huffCodes[i];
        if(code == NULL || barlen(bits) != barlen(code)){
            continue;
        }
        for(int j = 0; j < barlen(bits); j++){
            if(bit_array_get_bit(code, j) != bit_array_get_bit(bits, j)){
                success = 0;
                break;
            }
        }
        if(success){
            return i;
        }
    }
    return 0;
}

//TODO: fd resolving
int random_access_read_symbol(int fd, unsigned long i) {
    BIT_ARRAY * symCode = barcreate(ALPHABETSIZE);
    int len = getHuffmanCode(PAPFS_DATA->wavelet_root, i, symCode, 1);
    if (len == -1) {
        bardestroy(symCode);
        return -1;
    }
    bit_array_resize(symCode, len);

    char *str = (char *) malloc(len + 1);
    bit_array_to_str(symCode, str);
    //log_print("Found code: %s\n", symCode);

    uint8_t sym = decodeHuffmanCode(symCode);
    bardestroy(symCode);
    return sym;
}

void ra_write() {

}