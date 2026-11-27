#include <assert.h>
extern unsigned char __VERIFIER_nondet_uchar(void);

int main(void)
{
  unsigned char a[3];
  for (int i = 0; i < 3; i++)
    a[i] = __VERIFIER_nondet_uchar();
  assert(a[0] + a[1] + a[2] <= 765);
  return 0;
}
// Imported from ESBMC regression/witnesses/test_case_generation/github_10821_safe
// Needs: --generate-testcase
// RUN: --generate-testcase --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
