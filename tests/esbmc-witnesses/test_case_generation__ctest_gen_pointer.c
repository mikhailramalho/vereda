#include <assert.h>

void *__VERIFIER_nondet_pointer(void);

int main()
{
  void *p = __VERIFIER_nondet_pointer();
  assert(p != 0);
  return 0;
}
// Imported from ESBMC regression/witnesses/test_case_generation/ctest_gen_pointer
// Needs: --generate-ctest-testcase CHECK_FILE esbmc-ctest/test_case.c contains static void\* const v\[\] CHECK_FILE esbmc-ctest/test_case.c absent static const void\*
// RUN: --generate-ctest-testcase
// MODE: FUTURE
// EXPECT: ^Generated CTest test case\: test_case\.c$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
