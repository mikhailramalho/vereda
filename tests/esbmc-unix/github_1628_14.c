#include <stdio.h>

int main(void)
{
  int x = 1;
  printf("%p\n", x); // invalid: pointer expected
}

// Imported from ESBMC regression/esbmc-unix/github_1628_14
// Needs: --printf-check
// RUN: --printf-check
// MODE: FUTURE
// EXPECT: ^  printf format specifier \%p requires pointer argument$
// Vereda: verdict none
