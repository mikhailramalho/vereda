/* #9354: fwrite starting one byte before its buffer. */
#include <stdio.h>

int main(void)
{
  char b[4] = {'a', 'b', 'c', 'd'};
  fwrite(b - 1, 1, 2, stdout);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_9354_fwrite_before_fail
// RUN: -m64
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: FAILED +\[fwrite\.array-bounds-violated\.[0-9]+\].*dereference failure: array bounds violated
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
