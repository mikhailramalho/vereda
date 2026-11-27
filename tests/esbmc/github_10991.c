#include <assert.h>
#include <string.h>

unsigned __VERIFIER_nondet_uint(void);

int main()
{
  char d[8] = {1, 1, 1, 1, 1, 1, 1, 1};
  unsigned off = __VERIFIER_nondet_uint(), n = __VERIFIER_nondet_uint();
  __VERIFIER_assume(off <= 8 && n <= 8 - off);
  memset(d + off, 7, n);
  unsigned j = __VERIFIER_nondet_uint();
  __VERIFIER_assume(j < 8);
  assert(d[j] == (off <= j && j < off + n ? 7 : 1));
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10991
// RUN: --unwind 2
// EXPECT: ^VERIFICATION SUCCESSFUL$
