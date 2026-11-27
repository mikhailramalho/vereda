#include <assert.h>
extern unsigned char __VERIFIER_nondet_uchar(void);

int main(void)
{
  unsigned char a[3];
  for (int i = 0; i < 3; i++)
    a[i] = __VERIFIER_nondet_uchar();
  assert(a[0] != 5 || a[1] != 6 || a[2] != 7);
  return 0;
}
// Imported from ESBMC regression/witnesses/test_case_generation/github_10821_ctest
// Needs: --generate-ctest-testcase CHECK_FILE esbmc-ctest/test_case.c contains static unsigned char const v\[\] = \{ 5, 6, 7 \};
// RUN: --generate-ctest-testcase --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
