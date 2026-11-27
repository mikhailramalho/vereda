#include <stdio.h>
struct Foo {};
int main(void)
{
  struct Foo f;
  char buf[100];
  snprintf(buf, 100, "%s\n", f);
}
// Imported from ESBMC regression/esbmc-unix/github_1628_7
// Needs: --printf-check
// RUN: --printf-check
// MODE: FUTURE
// EXPECT: ^  printf format specifier \%s requires pointer argument$
// Vereda: verdict none
