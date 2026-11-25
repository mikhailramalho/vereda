#include <stdio.h>
#include <assert.h>
int main()
{
  long s = 1000000000000000000;
  int x = printf("%ld\n", s);
  assert(x == 20);
}
// Imported from ESBMC regression/cstd/printf_bug_2
// RUN: --overflow-check -m32
// MODE: KNOWNBUG
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: \bdereference failure: NULL pointer\b
// Vereda: verdict FAILED
