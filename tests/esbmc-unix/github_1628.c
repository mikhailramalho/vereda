#include <stdio.h>

struct Foo {};

int main(int argc, char **argv)
{
  struct Foo f;
  printf("%s\n", f);
}
// Imported from ESBMC regression/esbmc-unix/github_1628
// Needs: --printf-check
// RUN: --printf-check
// MODE: FUTURE
// EXPECT: ^  printf format specifier \%s requires pointer argument$
// Vereda: verdict none
