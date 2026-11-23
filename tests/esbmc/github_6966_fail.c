#include <stdio.h>
#include <stdlib.h>

extern int __VERIFIER_nondet_int(void);

/* The success path still owns the buffer: dropping it is a genuine leak and
   must stay detected after the failure path stopped allocating (#6966). */
int main(void)
{
  const char *fmt = __VERIFIER_nondet_int() ? "%d" : "x%d";
  char *msg;
  int used = asprintf(&msg, fmt, 1);
  if (used < 0)
    return 0;
  return 0;
}
// Imported from ESBMC regression/esbmc/github_6966_fail
// Needs: --memory-leak-check --no-reachable-memory-leak --no-abnormal-memory-leak --malloc-zero-is-null
// RUN: --memory-leak-check --no-reachable-memory-leak --no-abnormal-memory-leak --malloc-zero-is-null --incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^\s*dereference failure: forgotten memory: dynamic_1_array$
// Vereda: verdict none
