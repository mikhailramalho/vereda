/* #9892: a pointer stored into a flexible array member of a global. */
#include <stdlib.h>
int __VERIFIER_nondet_int(void);
struct E { int *p; };
struct S { int n; struct E arr[]; };
struct S g;
int *h;
int main()
{
  int *q = malloc(sizeof(int));
  h = q;
  int x = __VERIFIER_nondet_int();
  if (x > 0 && x < 0)
    g.arr[0].p = q;
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9892
// Needs: --memory-leak-check --no-reachable-memory-leak
// RUN: --memory-leak-check --no-reachable-memory-leak
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: PASSED +\[main\.memory-leak\.1\]
// Vereda: verdict none
