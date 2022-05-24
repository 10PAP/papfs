#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "CUnit/Basic.h"
#include "compressor.h"
#include "fs_opers.h"
#include "params.h"
#include "log.h"

/* Pointer to the BIT_ARRAY used by the tests. */
static BIT_ARRAY* code = NULL;
static WaveletNode* root = NULL;

/* Pointer to the file used by the tests. */
static FILE* temp_file = NULL;
//static char fpath[PATH_MAX];

/* The suite initialization function.
 * Returns zero on success, non-zero otherwise.
 */
int init_suite1(void)
{
   // creates bit_array for tests
   code = barcreate(12);
   bit_array_from_str(code, "100110010100");
   // creates the whole Wavelet-tree for tests
   root = malloc(sizeof(WaveletNode));
   if(!root){
      return -1;
   }
   root->bitmap = barcreate(4);
   bit_array_from_str(root->bitmap, "1001");
   root->left = malloc(sizeof(WaveletNode));
   if(!root->left){
      return -1;
   }
   root->right = malloc(sizeof(WaveletNode));
   if(!root->right){
      return -1;
   }
   root->left->bitmap = barcreate(2);
   bit_array_from_str(root->left->bitmap, "10");
   root->left->left = NULL;
   root->left->right = NULL;

   root->right->bitmap = barcreate(2);
   bit_array_from_str(root->right->bitmap, "00");
   root->right->left = NULL;
   root->right->right = NULL;


   if ((temp_file = fopen("alabarda.txt", "w+")) == NULL) {
      return -1;
   }
   fprintf(temp_file, "some test data");
    fflush(temp_file);

   PAPFS_DATA = malloc(sizeof(struct fs_state));
   if(!PAPFS_DATA){
      return -1;
   }
   
   PAPFS_DATA->logfile = log_open();
   PAPFS_DATA->rootdir = malloc(sizeof(char) * PATH_MAX);

   strcpy(PAPFS_DATA->rootdir, "./");
   printf("root = %s\n", PAPFS_DATA->rootdir);
   
  
   for(int i = 0 ; i < ALPHABETSIZE ; i++){
      PAPFS_DATA->huffCodes[i] = 0;
   }
   PAPFS_DATA->huffCodes['a'] = barcreate(2);
   PAPFS_DATA->huffCodes['b'] = barcreate(2);
   PAPFS_DATA->huffCodes['c'] = barcreate(2);
   bit_array_from_str(PAPFS_DATA->huffCodes['a'], "10");
   bit_array_from_str(PAPFS_DATA->huffCodes['b'], "01");
   bit_array_from_str(PAPFS_DATA->huffCodes['c'], "00");
   
   return 0;
}

void free_tree(WaveletNode* node){
   if(node->left){
      free_tree(node->left);
   }
   if(node->right){
      free_tree(node->right);
   }
   free(node);
}
/* The suite cleanup function.
 * Returns zero on success, non-zero otherwise.
 */
int clean_suite1(void)
{
   if (fclose(temp_file) != 0) {
      return -1;
   } else {
      temp_file = NULL;
   }

    if (remove("alabarda.txt") != 0) {
        return -1;
    }
    if (remove("test_pipe") != 0) {
        return -1;
    }

   bardestroy(code);
   free_tree(root);
   for(int i = 0 ; i < ALPHABETSIZE ; i++){
      if(PAPFS_DATA->huffCodes[i])
         bardestroy(PAPFS_DATA->huffCodes[i]);
   }
   return 0;
}


void testBinaryRank(void)
{
   int answers[] = {1, 1, 1, 2, 3, 3, 3, 4, 4, 5, 5, 5};
   for(int i = 0 ; i <= 11 ; i++){
      CU_ASSERT(rank(1, i, code) == answers[i]);
      CU_ASSERT(rank(0, i, code) == i + 1 - answers[i]);
   }
}

void testGetHuffmanCode(void)
{
   char* string_answers[] = {"10", "01", "00", "10"};
   BIT_ARRAY* real = barcreate(2);
   for(int i = 0 ; i < 4 ; i++){
      getHuffmanCode(root, i, real, 1);
      char string_real[3] = {0, 0, 0};
      bit_array_to_str(real, string_real);
      CU_ASSERT_STRING_EQUAL(string_real, string_answers[i]);
   }
   bardestroy(real);
}

void testGetTreeRank(void)
{
   BIT_ARRAY* code = barcreate(2);
   bit_array_from_str(code, "10");
   CU_ASSERT(getTreeRank(root, 1, code, 0) == 1);
   CU_ASSERT(getTreeRank(root, 4, code, 0) == 2);
   bit_array_from_str(code, "00");
   CU_ASSERT(getTreeRank(root, 1, code, 0) == 0);
   CU_ASSERT(getTreeRank(root, 2, code, 0) == 0);
   CU_ASSERT(getTreeRank(root, 3, code, 0) == 1);
   CU_ASSERT(getTreeRank(root, 4, code, 0) == 1);
   bardestroy(code);
}

void testDecodeHuffmanCode(void)
{
   BIT_ARRAY* code = barcreate(2);
   bit_array_from_str(code, "10");
   CU_ASSERT(decodeHuffmanCode(code) == 'a');
   bit_array_from_str(code, "01");
   CU_ASSERT(decodeHuffmanCode(code) == 'b');
   bit_array_from_str(code, "00");
   CU_ASSERT(decodeHuffmanCode(code) == 'c');
   bardestroy(code);
}

void testGetAttr(void) {
    struct stat statbuf;
    int retstat = PAPFS_getattr("alabarda.txt", &statbuf);

    CU_ASSERT(!retstat)
    CU_ASSERT(statbuf.st_size == 14)
    CU_ASSERT(statbuf.st_uid == getuid())
    CU_ASSERT(statbuf.st_gid == getgid())
}

void testMknod(void) {
    // create pipe
    mode_t mode = S_IRUSR | S_IWUSR | S_IFIFO;

    int retstat = PAPFS_mknod("test_pipe", mode, 0);
    CU_ASSERT(retstat >= 0)

    // get attributes
    struct  stat statbuf;
    int rretstat = PAPFS_getattr("test_pipe", &statbuf);

    CU_ASSERT(rretstat == 0)
    CU_ASSERT(statbuf.st_uid == getuid())
    CU_ASSERT(statbuf.st_mode = S_IRUSR | S_IWUSR | S_IFIFO) // check mode
}

void testUnlink(void) {
    const char * path = "remove_me.txt";
    // create file
    FILE * unlink_file = fopen(path, "w+");
    fprintf(unlink_file, "some important information");
    fclose(unlink_file);

    int retstat = PAPFS_unlink(path);
    CU_ASSERT(retstat == 0)

    // check that file doesn't exist
    CU_ASSERT(access(path, F_OK) != 0)
}

void testRename(void) {
    const char * path = "rename_me.txt";
    FILE * rename_file = fopen(path, "w+");
    fprintf(rename_file, "some data data data");
    fclose(rename_file);

    const char * new_path = "new_rename_me.txt";
    int retstat = PAPFS_rename(path, new_path);
    CU_ASSERT(retstat == 0)

    rename_file = fopen(new_path, "w+");
    CU_ASSERT(rename_file != NULL)

    remove(path);
}

void testUtime(void) {
    const char * path = "utime.txt";
    FILE * utime_file = fopen(path, "w+");
    fprintf(utime_file, "data data data\n");
    fclose(utime_file);

    // get current modification time
    struct stat statbuf;
    PAPFS_getattr(path, &statbuf);
    time_t time1 = statbuf.st_mtime;

    // change modification time to epoch
    struct utimbuf utime;
    utime.actime = 0;
    utime.modtime = 0;
    PAPFS_utime(path, &utime);

    // and again get modification time
    PAPFS_getattr(path, &statbuf);
    time_t time2 = statbuf.st_mtime;

    CU_ASSERT(time1 != time2)

    remove(path);
}

/* The main() function for setting up and running the tests.
 * Returns a CUE_SUCCESS on successful running, another
 * CUnit error code on failure.
 */
int main()
{
   CU_pSuite pSuite = NULL;

   /* initialize the CUnit test registry */
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

   /* add a suite to the registry */
   pSuite = CU_add_suite("PAPFS tests", init_suite1, clean_suite1);
   if (NULL == pSuite) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   // COMPRESSION TESTS
   if ((NULL == CU_add_test(pSuite, "test of rank()", testBinaryRank)) ||
       (NULL == CU_add_test(pSuite, "test of getHuffmanCode()", testGetHuffmanCode)) ||
       (NULL == CU_add_test(pSuite, "test of getTreeRank()", testGetTreeRank)) || 
       (NULL == CU_add_test(pSuite, "test of decodeHuffmanCode()", testDecodeHuffmanCode)))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   // FILESYSTEM OPERATIONS TESTS
   if ((NULL == CU_add_test(pSuite, "test of PAPFS_getattr()", testGetAttr)) ||
       (NULL == CU_add_test(pSuite, "test of PAPFS_mknod()", testMknod)) ||
       (NULL == CU_add_test(pSuite, "test of PAPFS_unlink()", testUnlink)) ||
       (NULL == CU_add_test(pSuite, "test of PAPFS_rename()", testRename)) ||
       (NULL == CU_add_test(pSuite, "test of PAPFS_utime()", testUtime)))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* Run all tests using the CUnit Basic interface */
   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();
   CU_cleanup_registry();
   return CU_get_error();
}
