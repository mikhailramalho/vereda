#include <assert.h>
typedef int v4i __attribute__((__vector_size__(16)));
extern unsigned __VERIFIER_nondet_uint(void);
extern int __VERIFIER_nondet_int(void);
int main(void)
{
  v4i g[2][3] = {};
  unsigned i = __VERIFIER_nondet_uint(), j = __VERIFIER_nondet_uint(), m = __VERIFIER_nondet_uint();
  unsigned i2 = __VERIFIER_nondet_uint(), j2 = __VERIFIER_nondet_uint(), m2 = __VERIFIER_nondet_uint();
  int n = __VERIFIER_nondet_int();
  __VERIFIER_assume(i < 2 && j < 3 && m < 4 && n != 0);
  __VERIFIER_assume(i2 < 2 && j2 < 3 && m2 < 4);
  g[i][j][m] = n;
  assert(g[i][j][m] == n && ((i == i2 && j == j2 && m == m2) || g[i2][j2][m2] == 0));
  return 0;
}
// Imported from ESBMC regression/esbmc/array_of_vector_symbolic
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
