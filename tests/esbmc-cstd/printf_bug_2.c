#include <stdio.h>
#include <assert.h>
int main()
{
  long s = 1000000000000000000;
  int x = printf("%ld\n", s);
  assert(x == 20);
}
// Imported from ESBMC regression/cstd/printf_bug_2
// Needs: REQUIRES arch32
// RUN: --overflow-check --32
// MODE: KNOWNBUG
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: \bdereference failure: NULL pointer\b
// Vereda: verdict none
