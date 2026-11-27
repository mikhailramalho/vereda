// #9358: with --printf-check, %s on an array that holds no NUL reads past it.
#include <stdio.h>

int main(void)
{
  char tag[3] = {'a', 'b', 'c'};
  printf("%s\n", tag);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9358_fail
// Needs: --printf-check
// RUN: --printf-check --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^  FAILED .* printf %s argument is not NUL-terminated within its object$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
