#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huffman.h"
#include <tgmath.h>
#include <stdint.h>
#include <sys/types.h>

#define CVECTOR_LOGARITHMIC_GROWTH
#include "cvector.h"
#include "bitarray/bit_array.h"
#include "bitarray/bar.h"
#include "alph.h"


typedef char WORD;

char* huffCodes[ALPHABETSIZE]; 

typedef struct WaveletTreeNode {
  cvector_vector_type(char) bitmap;
  struct WaveletTreeNode *left, *right;
} WaveletTreeNode;

WaveletTreeNode* waveletTreeNode(){
  WaveletTreeNode* node = malloc(sizeof(WaveletTreeNode));
  node->bitmap = NULL;
  node->left = NULL;
  node->right = NULL;
  return node;
}

void processCode(WaveletTreeNode* node, unsigned char c, int lvl){
  if(huffCodes[c][lvl] == '0'){
     cvector_push_back(node->bitmap, 0);
     if(lvl == strlen(huffCodes[c]) - 1)
      return;
     if(node->left == NULL)
      node->left = waveletTreeNode();
     processCode(node->left, c, lvl+1); 
  }
  else{
     cvector_push_back(node->bitmap, 1);
     if(lvl == strlen(huffCodes[c]) - 1)
      return;
     if(node->right == NULL)
      node->right = waveletTreeNode();
     processCode(node->right, c, lvl+1); 
  }
}

WaveletTreeNode* buildWaveletTree(unsigned char* seq, int l){
  WaveletTreeNode* root = waveletTreeNode();
  for(int i = 0 ; i < l ; i++){
    processCode(root, seq[i], 0);
  }
  return root;
}

// returns size of a tree in bits actually
int printTree(WaveletTreeNode* root)
{
    int res = 0;
    for(int i = 0 ; i < cvector_size(root->bitmap) ; i++){
      //printf("%d", root->bitmap[i]);
    }
    res += sizeof(WaveletTreeNode) * 8;
    res += cvector_size(root->bitmap);

    //printf("\n");

    if (root->left != NULL) {
        res += printTree(root->left);
    }
    if (root->right != NULL) {
        res += printTree(root->right);
    }
    return res;
}
  
static unsigned int rank(unsigned int bit, unsigned int i, cvector_vector_type(char) bitmap){
  unsigned int res = 0;
  for(int j = 0 ; j <= i ; j++)
    if(bitmap[j] == bit)
      res++;
  return res;
}

static void* getHuffmanCode(WaveletTreeNode* root, unsigned int i, cvector_vector_type(char) resBits){
  char bit = root->bitmap[i];
  unsigned int position = rank(bit, i, root->bitmap) - 1;
  WaveletTreeNode* next = (!bit)?root->left:root->right;
  cvector_push_back(resBits, bit);
  if(next == NULL)
    return resBits;
  return getHuffmanCode(next, position, resBits);
}

static char decodeHuffmanCode(cvector_vector_type(char) bits){
  int size = sizeof(huffCodes) / sizeof(huffCodes[0]);
  for(unsigned char i = 0 ; i < size ; i++){  
    int j = 0;
    char success = 1;
    char* s = huffCodes[i];
    if(s == NULL){
      continue;
    }
    for(; *s && j < cvector_size(bits); s++, j++){
      if(*s - '0' != bits[j]){
        success = 0;
        break;
      }
    }
    if(success){
      return i;
    }
  }
}


void freeTree(WaveletTreeNode* node){
  if(node->left){
    freeTree(node->left);
  }
  if(node->right){
    freeTree(node->right);
  }
  cvector_free(node->bitmap);
  free(node);
}

void serializeAll(WaveletTreeNode * root, int fd);

int freqs[ALPHABETSIZE];
unsigned char arr[ALPHABETSIZE];
int compress(unsigned char* buffer, size_t n, int fd){
  
  for(int i = 0 ; i < n ; i++)
    freqs[buffer[i]]++;

  for(int i = 0 ; i < ALPHABETSIZE ; i++)
    arr[i] = i;

  /*int n = 20;
  unsigned char * buffer = "alabar_a_la_alabarda";
  unsigned char arr[] = {'_', 'a', 'b', 'd', 'l', 'r'}; 
  int freqs[] = {3, 9, 2, 1, 3, 2};*/

  int sfreqs = 0;
  for(int i = 0 ; i < ALPHABETSIZE ; i++)
    sfreqs += freqs[i];

  int size = sizeof(arr) / sizeof(arr[0]);

  HuffmanCodes(arr, freqs, size);
  double entropy = 0;
  for(int i = 0 ; i < ALPHABETSIZE ; i++)
    if(freqs[i] > 0)
      entropy += -(freqs[i] / (double)sfreqs) * log2(freqs[i] / (double)sfreqs);
  printf("Huffman codes constructed successfully, entropy: %f\n", (entropy * n) / 8);

  WaveletTreeNode* root = buildWaveletTree(buffer, n);
  printf("Wavelet tree constructed successfully\n");
  int compressed_size = printTree(root);
  compressed_size += sizeof(huffCodes) * 8;
  for(int i = 0 ; i < ALPHABETSIZE ; i++)
    if(huffCodes[i] != NULL)
      compressed_size += strlen(huffCodes[i]) * 8;

  printf("compressed_size, bytes: %d\n", compressed_size / 8);

  serializeAll(root, fd);

  free(buffer);
  freeTree(root);

  return 0;
}

/*
 *  bitmap - reference to serialized bitmap in the file
 *  hasChildren - 1 if node has children, 0 otherwise
 */
#pragma pack(push, 1)
typedef struct SerialNode {
  uint32_t bitmap;
  uint8_t hasChildren;
} SerialNode;
#pragma pack(pop)

int32_t wavelet_tree_size(WaveletTreeNode * node) {
  int res = 1;

  if (node->left != NULL) {
    res += wavelet_tree_size(node->left);
  }

  if (node->right != NULL) {
    res += wavelet_tree_size(node->right);
  }

  return res;
}

/*
 * current bitmap offset
 */
off_t bitmap_offset;

/*
 * TODO: now I use bitarrays only for serialization. What about using them for
 * representing bitmaps in ram?
 */
uint64_t serializeBitmap(FILE * out, cvector_vector_type(char) bitmap) {
  off_t saved_offset = ftell(out);
  fseek(out, bitmap_offset, SEEK_SET);

  // here we construct bitarray and serialize it
  size_t size = cvector_size(bitmap);
  BIT_ARRAY * bitmap_arr = barcreate(size);

  // fill bitarray with values from cvector
  for (int i = 0; i < size; i++) {
    if (bitmap[i] == 1) {
      barset(bitmap_arr, i);
    } else {
      barclr(bitmap_arr, i);
    }
  }

  uint64_t bytes_written = bit_array_save(bitmap_arr, out, 4); //or 8 for big files

  bardestroy(bitmap_arr);
  fseek(out, saved_offset, SEEK_SET);
  return bytes_written;
}

int debug_counter = 0;
void serializeTree(FILE * out, WaveletTreeNode * node) {
  SerialNode sNode;

  //printf("%d node left --> %p\n", debug_counter, node->left);
  //printf("%d node right --> %p\n", debug_counter, node->right);

  // add info about children
  sNode.hasChildren = 0;
  if (node->left != NULL) {
    sNode.hasChildren |= 0b00000001;
  }
  if (node->right != NULL) {
    sNode.hasChildren |= 0b00000010;
  }

  // add info about bitmap
  sNode.bitmap = bitmap_offset;
  serializeBitmap(out, node->bitmap);
  bitmap_offset += 4 + roundup_bits2bytes(cvector_size(node->bitmap));

  fwrite(&sNode, sizeof(SerialNode), 1, out);

  if ((sNode.hasChildren & 0b00000001) > 0) {
    debug_counter++;
    serializeTree(out, node->left);
  }
  if ((sNode.hasChildren & 0b00000010) > 0) {
    debug_counter++;
    serializeTree(out, node->right);
  }
}

off_t serializeHuffmanCodes(FILE * out) {
  off_t huff_offset = 1;
  printf("Codes:\n");
  for (int code = 0; code < ALPHABETSIZE; code++) {
    if (huffCodes[code] == NULL) {
      printf("Found null\n");
      continue;
    } else {
      printf("code[%d] len=%ld: %s\n", code, strlen(huffCodes[code]), huffCodes[code]);
    }

    int code_size = strlen(huffCodes[code]);
    BIT_ARRAY * barr = barcreate(code_size);

    for (int b = 0; b < code_size; b++) {
      if (huffCodes[code][b] == '0') {
        barclr(barr, b);
      }
      if (huffCodes[code][b] == '1') {
        barset(barr, b);
      }
    }

    huff_offset += bit_array_save(barr, out, 1);

    bardestroy(barr);
  }
  return huff_offset;
}

void serializeAll(WaveletTreeNode * root, int fd) {
  FILE * output = fdopen(fd, "wb");

  char compressed = 1;
  fwrite(&compressed, 1, 1, output);

  // serialize huffman codes
  bitmap_offset = serializeHuffmanCodes(output);

  // serialize wavelet-tree
  bitmap_offset += sizeof(SerialNode) * wavelet_tree_size(root);
  printf("Nodes count: %d\n", wavelet_tree_size(root));
  printf("Bitmap offset: %ld\n", bitmap_offset);
  serializeTree(output, root);
}
