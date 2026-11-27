#include <stdio.h>
int main(void)
{
  const char *s = "hello";
  int x = 42;
  printf("%s %d %c\n", s, x, 'a');
}
// Imported from ESBMC regression/esbmc-unix/github_1628_8
// Needs: --printf-check
// RUN: --printf-check --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
