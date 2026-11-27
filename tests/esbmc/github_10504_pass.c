/* #10504: no call trace is printed when no property is violated. */
#include <assert.h>
int __VERIFIER_nondet_int(void);
int g;
void setup(int v) { g = v; }
void check(void) { assert(g != 3); }
int main(void)
{
  int v = __VERIFIER_nondet_int();
  __VERIFIER_assume(v != 3);
  setup(v);
  check();
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10504_pass
// Needs: --show-funccall-trace
// RUN: --show-funccall-trace
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: (?s)\A(?!.*Function call trace:)
// Vereda: verdict none
