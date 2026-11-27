/* esbmc/esbmc#10920: gmtime's operational model assumes ranges on its own
   nondets; they are not program inputs. */
#include <time.h>
int __VERIFIER_nondet_int();
int main()
{
  time_t t = 0;
  struct tm *p = gmtime(&t);
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assume(x >= 0 && x <= 3);
  __VERIFIER_assert(x != 2); // x != 2
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10920_library_nondets
// Needs: --all-witnesses
// RUN: --all-witnesses --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^\[Counterexamples - 1 witnesses\]$
// EXPECT: enumeration stopped: UNSAT after 1 witnesses
// EXPECT: ^    #1 : \[0\] = 2$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
