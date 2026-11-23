#include <stdio.h>
int main()
{
  char *p = "abc";
  sscanf("x", "%c", p);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10812_scanf_literal_fail
// EXPECT: ^  dereference failure: write access to string constant$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^  dereference failure: write access to string constant$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
