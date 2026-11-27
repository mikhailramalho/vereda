#include <assert.h>
typedef int v4i __attribute__((__vector_size__(16)));
extern unsigned __VERIFIER_nondet_uint(void);
int main(void)
{
  unsigned n = __VERIFIER_nondet_uint();
  __VERIFIER_assume(n > 1 && n < 4);
  v4i g[n][2];
  unsigned i = __VERIFIER_nondet_uint(), j = __VERIFIER_nondet_uint(), m = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i < n && j < 2 && m < 4);
  g[i][j] = (v4i){0, 0, 0, 0};
  g[i][j][m] = 6;
  assert(g[i][j][(m + 1) % 4] == 6);
  return 0;
}
// Imported from ESBMC regression/esbmc/array_of_vector_vla_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
