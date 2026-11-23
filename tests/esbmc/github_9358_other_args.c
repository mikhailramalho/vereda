// #9358: --printf-check claims no NUL for a non-%s conversion or for a %s
// argument that may point into either of two arrays.
#include <stdio.h>

_Bool __VERIFIER_nondet_bool(void);

int main(void)
{
  char a[3] = {'a', 0, 0};
  char b[2] = {'b', 0};
  char *s = __VERIFIER_nondet_bool() ? a : b;
  long n = 7;
  printf("%d %ld %s\n", 1, n, s);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9358_other_args
// Needs: --printf-check
// RUN: --printf-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
