/* esbmc/esbmc#10920: k[0] is also read inside the isdigit model, which does
   not make it a library input. */
#include <ctype.h>
int __VERIFIER_nondet_int();
int main()
{
  int k[1];
  k[0] = __VERIFIER_nondet_int();
  __VERIFIER_assume(k[0] >= 0 && k[0] < 4);
  int r = isdigit(k[0]);
  __VERIFIER_assert(k[0] < 2 || r); // k[0] < 2 || r
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10920_library_argument
// Needs: --all-witnesses
// RUN: --all-witnesses
// MODE: FUTURE
// EXPECT: ^\[Counterexamples - 2 witnesses\]$
// EXPECT: enumeration stopped: UNSAT after 2 witnesses
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
