#include <stdio.h>

int main()
{
  // empty format string
  printf("");
}
// Imported from ESBMC regression/cstd/printf_6
// Needs: --k-induction
// RUN: --k-induction
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
