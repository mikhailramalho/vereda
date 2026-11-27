#include <stdio.h>

int main(void)
{
  printf("%s\n", "hello", 123, 3.14);
}

// Imported from ESBMC regression/esbmc-unix/github_1628_13
// Needs: --printf-check
// RUN: --printf-check --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
