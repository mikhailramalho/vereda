#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>

/* asprintf may fail even for a constant format, leaving msg untouched, so
   writing through it unchecked is a NULL dereference (#9159). */
int main(void)
{
  char *msg = NULL;
  int used = asprintf(&msg, "hi");
  (void)used;
  msg[0] = 'x';
  free(msg);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9159_unchecked_fail
// RUN: -m64 --no-incremental-bmc
// EXPECT: ^\s*dereference failure: NULL pointer$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^\s*dereference failure: NULL pointer$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
