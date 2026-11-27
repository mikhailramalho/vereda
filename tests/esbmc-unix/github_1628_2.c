#include <stdio.h>
int main(void)
{
  char c = 'x';
  printf("%s\n", c);
}
// Imported from ESBMC regression/esbmc-unix/github_1628_2
// Needs: --printf-check
// RUN: --printf-check --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^  printf format specifier \%s requires pointer argument$
// Vereda: verdict none
