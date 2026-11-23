#include <stdio.h>
union Data {
  int i;
  float f;
};
int main(void)
{
  union Data d;
  d.i = 42;
  printf("%s\n", d);
}
// Imported from ESBMC regression/esbmc-unix/github_1628_9
// Needs: --printf-check
// RUN: --printf-check
// MODE: FUTURE
// EXPECT: ^  printf format specifier \%s requires pointer argument$
// Vereda: verdict none
