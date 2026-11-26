#include <assert.h>
#include <stdint.h>
int __VERIFIER_nondet_int(void);
uintptr_t __VERIFIER_nondet_addr(void);
int main(void)
{
  int a[2] = {0, 1};
  uintptr_t u = __VERIFIER_nondet_addr();
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assume(x >= 0 && x < 4);
  int *p = (int *)u;
  assert(p != (int *)((uintptr_t)a + sizeof(int)) || x != 1);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7855-all-witnesses_fail
// Needs: --all-witnesses --max-witnesses 2
// RUN: -m64 --all-witnesses --max-witnesses 2
// EXPECT: ^\[Counterexamples - 2 witnesses\]
// EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict none
