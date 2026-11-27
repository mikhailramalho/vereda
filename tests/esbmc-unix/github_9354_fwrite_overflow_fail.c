/* #9354: size * nitems wraps size_t, so no object holds the bytes read. */
#include <stdio.h>

int main(void)
{
  char b[4] = {'a', 'b', 'c', 'd'};
  fwrite(b, (1ULL << 63) + 1, 2, stdout);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_9354_fwrite_overflow_fail
// RUN: -m64
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: FAILED +\[fwrite\.array-bounds-violated\.[0-9]+\].*dereference failure: array bounds violated
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
