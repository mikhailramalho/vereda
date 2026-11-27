#include <stdio.h>
int main(void)
{
  int x = 123;
  printf("%10.5s\n", x);
}
// Imported from ESBMC regression/esbmc-unix/github_1628_11
// Needs: --printf-check
// RUN: --printf-check --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^  printf format specifier \%s requires pointer argument$
// Vereda: verdict none
