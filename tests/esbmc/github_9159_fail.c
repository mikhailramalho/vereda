#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>

/* The asprintf buffer holds used + 1 bytes: msg[used + 1] is past it (#9159). */
int main(void)
{
  char *msg;
  int used = asprintf(&msg, "%s", "hello");
  if (used < 0)
    return 0;
  msg[used + 1] = '\n';
  free(msg);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9159_fail
// Needs: --memory-leak-check
// RUN: --64 --memory-leak-check
// MODE: FUTURE
// EXPECT: ^\s*dereference failure: array bounds violated: heap object$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
