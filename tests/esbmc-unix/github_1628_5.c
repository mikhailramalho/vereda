#include <stdio.h>
int main(void)
{
  char arr[] = "test";
  printf("%s\n", arr);
}
// Imported from ESBMC regression/esbmc-unix/github_1628_5
// Needs: --printf-check
// RUN: --printf-check --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
