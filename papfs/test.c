#include <stdio.h>
#include <string.h>
#include "CUnit/Basic.h"
#include "compressor.h"
/* Pointer to the BIT_ARRAY used by the tests. */
static BIT_ARRAY* code = NULL;

/* The suite initialization function.
 * Returns zero on success, non-zero otherwise.
 */
int init_suite1(void)
{
   code = barcreate(12);
   bit_array_from_str(code, "100110010100");
   return 0;
}

/* The suite cleanup function.
 * Returns zero on success, non-zero otherwise.
 */
int clean_suite1(void)
{
   bardestroy(code);
   return 0;
}


void testBinaryRank(void)
{
   int answers[] = {1, 1, 1, 2, 3, 3, 3, 4, 4, 5, 5, 5};
   for(int i = 0 ; i <= 11 ; i++)
      CU_ASSERT(rank(1, i, code) == answers[i]);
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
   pSuite = CU_add_suite("Suite_1", init_suite1, clean_suite1);
   if (NULL == pSuite) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the suite */
   /* NOTE - ORDER IS IMPORTANT - MUST TEST fread() AFTER fprintf() */
   if ((NULL == CU_add_test(pSuite, "test of rank()", testBinaryRank)))
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
