/* #9354: fputs reads its argument up to the terminating null character. */
#include <stdio.h>

int main(void)
{
  char tag[3] = {'a', 'b', 'c'};
  fputs(tag, stdout);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_9354_fputs_fail
// RUN: -m64 --unwind 8
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: FAILED +\[strlen\.array-bounds-violated\.[0-9]+\].*dereference failure: array bounds violated
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
