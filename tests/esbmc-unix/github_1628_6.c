#include <stdio.h>
struct Bar {};
int main(void)
{
  int x = 10;
  struct Bar b;
  printf("%d %s\n", x, b);
}
// Imported from ESBMC regression/esbmc-unix/github_1628_6
// Needs: --printf-check
// RUN: --printf-check
// MODE: FUTURE
// EXPECT: ^  printf format specifier \%s requires pointer argument$
// Vereda: verdict none
