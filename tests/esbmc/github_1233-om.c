#include <string.h>

int main()
{
  char dst[4];
  strcpy(dst, "too long");
  return 0;
}
// Imported from ESBMC regression/esbmc/github_1233-om
// Needs: --show-stacktrace
// RUN: --show-stacktrace
// MODE: FUTURE
// EXPECT: ^Stack trace:$
// EXPECT: ^  strcpy at file .*github_1233\-om\.c line 6 column 3 function main$
// EXPECT: ^  main at file .*github_1233\-om\.c line 3 column 1$
// EXPECT: ^  dereference failure: array bounds violated$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
