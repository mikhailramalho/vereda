#include <assert.h>
int __VERIFIER_nondet_int(void);
int main(void)
{
  int v = __VERIFIER_nondet_int();
  switch (v)
  {
  case 1 ... 5:
    assert(v != 3);
    break;
  }
  return 0;
}
// Imported from ESBMC regression/esbmc/github_8676_case_range_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
