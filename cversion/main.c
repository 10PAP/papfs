#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huffman.h"

#define CVECTOR_LOGARITHMIC_GROWTH
#include "cvector.h"

#define BYTESWORDSIZE (1)
#define ALPHABETSIZE (256)
#define BLOCKSIZE (1024)

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


void printTree(WaveletTreeNode* root)
{
    for(int i = 0 ; i < cvector_size(root->bitmap) ; i++){
      printf("%d", root->bitmap[i]);
    }
    printf("\n");

    if (root->left != NULL) {
        printTree(root->left);
    }
    if (root->right != NULL) {
        printTree(root->right);
    }
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

int main(){
  FILE * infile = fopen("test_data", "rb");
  unsigned char * buffer = (char*)malloc(BLOCKSIZE);
  fread(buffer, BLOCKSIZE, BYTESWORDSIZE, infile);
  fclose(infile);

  /*
  int freqs[ALPHABETSIZE] = {0};
  for(int i = 0 ; i < BLOCKSIZE ; i++)
    freqs[buffer[i]]++;

  unsigned char arr[256];
  for(int i = 0 ; i < 256 ; i++)
    arr[i] = i;
  */

  buffer = "alabar_a_la_alabarda";
  unsigned char arr[] = {'_', 'a', 'b', 'd', 'l', 'r'}; 
  int freqs[] = {3, 9, 2, 1, 3, 2};

  int size = sizeof(arr) / sizeof(arr[0]);

  HuffmanCodes(arr, freqs, size);

  WaveletTreeNode* root = buildWaveletTree(buffer, strlen(buffer));
  printTree(root);

  cvector_vector_type(char) code = NULL;
  for(unsigned int pos = 0 ; pos < 20 ; pos++){
    code = getHuffmanCode(root, pos, code);
    printf("%c", decodeHuffmanCode(code));
    cvector_free(code);
    code = NULL;
  }
  printf("\n");

  return 0;
}
