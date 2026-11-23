#include <assert.h>
extern int __VERIFIER_nondet_int(void);
int main(void)
{
  int c = __VERIFIER_nondet_int();
  double u = (c != 0);
  assert((int)u == (c != 0 ? 1 : 0));
  return 0;
}
// Imported from ESBMC regression/esbmc/github_562_bool
// Needs: --fixedbv
// RUN: --fixedbv
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
