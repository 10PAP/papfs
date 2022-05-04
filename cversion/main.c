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

int m[256];

void init_m(){
  m[0]=1; m[1]=1; m[2]=1; m[3]=1; m[4]=1; m[5]=1; m[6]=1; m[7]=1; m[8]=1; m[9]=1; m[10]=1; m[11]=1; m[12]=1; m[13]=1; m[14]=1; m[15]=1; m[16]=1;
  m[17]=1; m[18]=1; m[19]=1; m[20]=1; m[21]=1; m[22]=1; m[23]=1; m[24]=1; m[25]=1; m[26]=1; m[27]=1; m[28]=1; m[29]=1; m[30]=1; m[31]=1; m[32]=1;
  m[33]=1; m[34]=1; m[35]=1; m[36]=1; m[37]=1; m[38]=1; m[39]=1; m[40]=1; m[41]=1; m[42]=1; m[43]=1; m[44]=1; m[45]=1; m[46]=1; m[47]=1; m[48]=1;
  m[49]=1; m[50]=1; m[51]=1; m[52]=1; m[53]=1; m[54]=1; m[55]=1; m[56]=1; m[57]=1; m[58]=1; m[59]=1; m[60]=1; m[61]=1; m[62]=1; m[63]=1; m[64]=1;
  m[65]=1; m[66]=1; m[67]=1; m[68]=1; m[69]=1; m[70]=1; m[71]=1; m[72]=1; m[73]=1; m[74]=1; m[75]=1; m[76]=1; m[77]=1; m[78]=1; m[79]=1; m[80]=1;
  m[81]=1; m[82]=1; m[83]=1; m[84]=1; m[85]=1; m[86]=1; m[87]=1; m[88]=1; m[89]=1; m[90]=1; m[91]=1; m[92]=1; m[93]=1; m[94]=1; m[95]=1; m[96]=1;
  m[97]=1; m[98]=1; m[99]=2; m[100]=2; m[101]=2; m[102]=2; m[103]=2; m[104]=2; m[105]=2; m[106]=2; m[107]=2; m[108]=2; m[109]=2; m[110]=2; m[111]=2;
  m[112]=2; m[113]=2; m[114]=2; m[115]=2; m[116]=2; m[117]=2; m[118]=2; m[119]=2; m[120]=2; m[121]=2; m[122]=2; m[123]=2; m[124]=2; m[125]=2; m[126]=2;
  m[127]=2; m[128]=2; m[129]=3; m[130]=3; m[131]=3; m[132]=3; m[133]=3; m[134]=3; m[135]=3; m[136]=3; m[137]=3; m[138]=3; m[139]=3; m[140]=3; m[141]=3;
  m[142]=3; m[143]=4; m[144]=4; m[145]=4; m[146]=4; m[147]=4; m[148]=4; m[149]=4; m[150]=4; m[151]=4; m[152]=4; m[153]=4; m[154]=4; m[155]=4; m[156]=4;
}

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
  
unsigned int rank(unsigned int bit, unsigned int i, cvector_vector_type(char) bitmap){
  unsigned int res = 0;
  for(int j = 0 ; j <= i ; j++)
    if(bitmap[j] == bit)
      res++;
  return res;
}

void* getHuffmanCode(WaveletTreeNode* root, unsigned int i, cvector_vector_type(char) resBits){
  char bit = root->bitmap[i];
  unsigned int position = rank(bit, i, root->bitmap) - 1;
  WaveletTreeNode* next = (!bit)?root->left:root->right;
  cvector_push_back(resBits, bit);
  if(next == NULL)
    return resBits;
  return getHuffmanCode(next, position, resBits);
}

char decodeHuffmanCode(cvector_vector_type(char) bits){
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

void serializeAll(WaveletTreeNode * root);

int freqs[ALPHABETSIZE];
unsigned char arr[ALPHABETSIZE];
int main(int argc, char** argv){
  init_m();
  FILE * infile = fopen(argv[1], "rb");
  printf("test %s\n", argv[1]);

  fseek(infile, 0L, SEEK_END);
  unsigned long n = ftell(infile);
  fseek(infile, 0L, SEEK_SET);
  printf("file_size: %lu\n", n);

  unsigned char * buffer = (char*) malloc(n);
  fread(buffer, n, 1, infile);
  fclose(infile);

  printf("File reading finished\n");

  
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

  serializeAll(root);

  cvector_vector_type(char) code = NULL;
  /*
  for(unsigned int pos = 0 ; pos < 20 ; pos++){
    code = getHuffmanCode(root, pos, code);
    printf("%c", decodeHuffmanCode(code));
    cvector_free(code);
    code = NULL;
  }
  printf("\n");
  */

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
  off_t huff_offset = 0;
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

void serializeAll(WaveletTreeNode * root) {
  FILE * output = fopen("data.bin", "wb");

  // serialize huffman codes
  bitmap_offset = serializeHuffmanCodes(output);

  // serialize wavelet-tree
  bitmap_offset += sizeof(SerialNode) * wavelet_tree_size(root);
  printf("Nodes count: %d\n", wavelet_tree_size(root));
  printf("Bitmap offset: %ld\n", bitmap_offset);
  serializeTree(output, root);
}