#include <stdio.h>
#include <assert.h>

/* As github_8083_symex_guard, through scanf. */
int main()
{
  int n = 1;
  scanf("%d", &n);
  __VERIFIER_assume(n >= 0 && n <= 2);
  int i = 0;
  while (i < n)
    ++i;
  assert(i <= 1);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_8083_symex_guard_scanf
// RUN: --unwind 4
// EXPECT: ^VERIFICATION FAILED$
