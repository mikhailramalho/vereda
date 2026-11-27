#include <assert.h>
#include <string.h>

unsigned __VERIFIER_nondet_uint(void);

struct s
{
  char a;
  char b;
};

__attribute__((annotate("__ESBMC_inf_size"))) char pool[1];

int main()
{
  unsigned n = __VERIFIER_nondet_uint();
  __VERIFIER_assume(n <= 2);

  char big[4097] = {0};
  memset(big + 4095, 5, n);
  assert(n < 2 || big[4096] == 5);

  struct s x = {1, 1};
  memset(&x, 0, n);
  assert(n < 2 || x.b == 0);

  unsigned k = __VERIFIER_nondet_uint();
  __VERIFIER_assume(k >= 2 && k <= 3);
  char vla[k];
  memset(vla, 6, n);
  assert(n < 2 || vla[1] == 6);

  memset(pool, 7, n);
  assert(n < 2 || pool[1] == 7);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10991_fallback
// RUN: --unwind 3
// EXPECT: ^VERIFICATION SUCCESSFUL$
