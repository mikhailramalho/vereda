/* Negative counterpart of github_7597-literal: the loop still has to fold for
 * this claim to be reached, and the symbolic element must not read back as
 * the literal beside it. */
#include <assert.h>

struct pair
{
  int n;
  int r;
};

int __VERIFIER_nondet_int(void);

int main(void)
{
  int v = __VERIFIER_nondet_int();
  struct pair s = {5, v};
  int c = 0;

  for (int i = 0; i < s.n; i++)
    c++;

  assert(s.r == 5);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7597-literal_fail
// RUN: --ub-pointer-check=false --ub-div-by-zero-check=false --vla-size-check=false
// EXPECT: ^VERIFICATION FAILED$
