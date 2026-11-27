// --dead-code-check forces a SUCCESSFUL verdict, so a genuine SAT violation
// from a symex-injected safety check (memory-leak here, also deadlock/race)
// would be silently masked. Reject the combination so a real leak is never
// hidden behind a dead-code run (issue #4495).
#include <stdlib.h>
int main(void)
{
  int *p = malloc(sizeof(int));
  return p ? 0 : 1;
}
// Imported from ESBMC regression/esbmc/cwe_dead_code_reject_leak_check
// Needs: --dead-code-check --memory-leak-check
// RUN: --dead-code-check --memory-leak-check --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^ERROR: --dead-code-check cannot be combined with --memory-leak-check$
// Vereda: verdict none
