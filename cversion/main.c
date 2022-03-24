#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huffman.h"
#include <tgmath.h>
#include <stdint.h>
#include <sys/types.h>

#define CVECTOR_LOGARITHMIC_GROWTH
#include "cvector.h"

#define ALPHABETSIZE (6)

char* huffCodes[256]; 

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

void serializeTree(WaveletTreeNode * root);

int main(int argc, char** argv){
  /*FILE * infile = fopen(argv[1], "rb");
  printf("test %s\n", argv[1]);

  //fseek(infile, 0L, SEEK_END);
  int n = 1024 * 1024 * 1024;//ftell(infile);
  //fseek(infile, 0L, SEEK_SET);
  printf("file_size: %d\n", n);*/

  /*unsigned char * buffer = (char*)malloc(n);
  fread(buffer, n, 1, infile);
  fclose(infile);*/

  /*int freqs[ALPHABETSIZE] = {0};
  for(int i = 0 ; i < n ; i++)
    freqs[buffer[i]]++;*/

  /*unsigned char arr[ALPHABETSIZE];
  for(int i = 0 ; i < ALPHABETSIZE ; i++)
    arr[i] = i;*/

  int n = 20;
  unsigned char * buffer = "alabar_a_la_alabarda";
  unsigned char arr[] = {'_', 'a', 'b', 'd', 'l', 'r'}; 
  int freqs[] = {3, 9, 2, 1, 3, 2};

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

  serializeTree(root);

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

typedef struct SerialNode {
  off_t bitmap;
  uint8_t children;
} SerialNode;

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

void serializeTree(WaveletTreeNode * root) {
  int size = wavelet_tree_size(root);
  printf("Nodes count: %d\n", size);

  FILE * output = fopen("data.bin", "wb");


}