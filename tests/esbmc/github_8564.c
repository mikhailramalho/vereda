#include <assert.h>
#include <stdio.h>

int __VERIFIER_nondet_int(void);
unsigned __VERIFIER_nondet_uint(void);

int main(void)
{
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assume(x >= 100 && x <= 999);
  assert(printf("v=%d\n", x) == 6);

  int n = __VERIFIER_nondet_int();
  __VERIFIER_assume(n > -100 && n <= -10);
  assert(fprintf(stderr, "%i", n) == 3);

  unsigned u = __VERIFIER_nondet_uint();
  __VERIFIER_assume(u >= 10000u && u < 100000u);
  assert(dprintf(1, "[%u]", u) == 7);
  assert(printf("%d", 4294967295u) == 2);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_8564
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict FAILED
