#include <stdbool.h>

// Regression for issue #6031: combining --termination with
// --k-induction-parallel produced non-deterministic, sometimes unsound
// VERIFICATION SUCCESSFUL. The program does not terminate (the while(true)
// loop never exits), so --termination must report VERIFICATION FAILED
// deterministically. --termination now takes priority over the parallel
// k-induction driver, which has no termination-property interpretation.
int a;
int main()
{
  while (true)
  {
    if (a == 0)
      a = 1;
    else if (a == 1)
      a = 2;
    else if (a == 2)
    {
      if (true)
        a = 3;
    }
    else
      __VERIFIER_error();
  }
  return 0;
}
// Imported from ESBMC regression/termination/k_induction_parallel_multi_property
// Needs: --termination --k-induction-parallel
// RUN: --termination --k-induction-parallel --multi-property
// MODE: FUTURE
// EXPECT: Both k-induction and --termination specified. Using --termination.
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
