/* #10504: the call trace names functions by source name and follows the
   violated-property block. */
#include <assert.h>
int __VERIFIER_nondet_int(void);
int g;
void setup(int v) { g = v; }
void check(void) { assert(g != 3); }
int main(void)
{
  setup(__VERIFIER_nondet_int());
  check();
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10504
// Needs: --show-funccall-trace
// RUN: --show-funccall-trace
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
