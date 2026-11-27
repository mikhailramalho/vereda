#include <stdio.h>
int main(void)
{
  int x = 42;
  printf("%s\n", x);
}
// Imported from ESBMC regression/esbmc-unix/github_1628_3
// Needs: --printf-check
// RUN: --printf-check
// MODE: FUTURE
// EXPECT: ^  printf format specifier \%s requires pointer argument$
// Vereda: verdict none
