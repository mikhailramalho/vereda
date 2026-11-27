#include <assert.h>
extern unsigned char __VERIFIER_nondet_uchar(void);

int main(void)
{
  unsigned char a[3];
  for (int i = 0; i < 3; i++)
    a[i] = __VERIFIER_nondet_uchar();
  assert(a[2] != 7);
  return 0;
}
// Imported from ESBMC regression/witnesses/test_case_generation/github_10821
// Needs: --generate-testcase CHECK_FILE testcase.xml contains <testcase coversError="true">\n<input>[0-9]+</input>\n<input>[0-9]+</input>\n<input>7</input>\n</testcase>
// RUN: --generate-testcase --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
