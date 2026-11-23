#include <stdio.h>
int main(void)
{
  float f = 3.14;
  printf("%s\n", f);
}
// Imported from ESBMC regression/esbmc-unix/github_1628_10
// Needs: --printf-check
// RUN: --printf-check
// MODE: FUTURE
// EXPECT: ^  printf format specifier \%s requires pointer argument$
// Vereda: verdict none
