// sscanf writes through each argument past the format, here &x, so the
// inductive step havocs x rather than proving the false assertion.
#include <stdio.h>

int main(void)
{
  int x = 0;
  char buf[4] = "7";
  unsigned i = 0;
  while (1)
  {
    if (i == 10)
      sscanf(buf, "%d", &x);
    if (i == 20)
      __VERIFIER_assert(x == 0); // sscanf parses a non-zero x
    i++;
  }
  return 0;
}
// Imported from ESBMC regression/k-induction/sscanf_in_loop_havoc_fail
// Needs: --k-induction --max-inductive-step 3
// RUN: --k-induction --max-inductive-step 3
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
