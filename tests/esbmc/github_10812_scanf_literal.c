#include <assert.h>
#include <stdio.h>
int main()
{
  char buf[4] = "abc";
  char *p = __VERIFIER_nondet_bool() ? "abc" : buf;
  __VERIFIER_assume(p == buf);
  sscanf("x", "%c", p);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10812_scanf_literal
// EXPECT: ^VERIFICATION SUCCESSFUL$
