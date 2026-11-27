#include <stdio.h>

int main(void)
{
  printf("%s %d\n", "hello");
}
// Imported from ESBMC regression/esbmc-unix/github_1628_12
// Needs: --printf-check
// RUN: --printf-check --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^  printf has more format specifiers than arguments$
// Vereda: verdict none
