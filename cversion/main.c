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
  

int main(){
  FILE * infile = fopen("test_data", "rb");
  unsigned char * buffer = (char*)malloc(BLOCKSIZE);
  fread(buffer, BLOCKSIZE, BYTESWORDSIZE, infile);
  fclose(infile);

  buffer = "alabar_a_la_alabarda";

  /*
  int freqs[ALPHABETSIZE] = {0};
  for(int i = 0 ; i < BLOCKSIZE ; i++)
    freqs[buffer[i]]++;
  */
  int freqs[] = {3, 9, 2, 1, 3, 2};

  // unsigned char arr[256];
  unsigned char arr[] = {'_', 'a', 'b', 'd', 'l', 'r'}; 
  //for(int i = 0 ; i < 256 ; i++)
  //  arr[i] = i;

  int size = sizeof(arr) / sizeof(arr[0]);

  HuffmanCodes(arr, freqs, size);

  WaveletTreeNode* root = buildWaveletTree(buffer, strlen(buffer));
  printTree(root);

  return 0;
}
