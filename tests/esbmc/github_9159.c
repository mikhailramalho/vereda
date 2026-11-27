#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>

/* asprintf allocates used + 1 bytes, so msg[used] is in bounds (#9159). */
int main(void)
{
  char *msg;
  int used = asprintf(&msg, "%s", "hello");
  if (used < 0)
    return 0;
  msg[used] = '\n';
  free(msg);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9159
// Needs: --memory-leak-check
// RUN: -m64 --memory-leak-check --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none
