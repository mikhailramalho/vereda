#include <stdio.h>
int main(void)
{
  const char *s = "hello";
  printf("%s\n", s);
}
// Imported from ESBMC regression/esbmc-unix/github_1628_4
// Needs: --printf-check
// RUN: --printf-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
