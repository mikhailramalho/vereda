#include <stdlib.h>

extern int __VERIFIER_nondet_int(void);

int **g;

void free_g(void)
{
  g = 0;
}

int main()
{
  g = malloc(sizeof(int *));
  if (!g)
    return 0;
  *g = malloc(sizeof(int));
  atexit(free_g);
  if (__VERIFIER_nondet_int())
    exit(1);
  free(*g);
  return 0;
}
// Imported from ESBMC regression/witnesses_validate/memsafety-violationwitness/exit_leak_in_model_wrong_line
// Needs: --memory-leak-check --no-reachable-memory-leak --no-abnormal-memory-leak --validate-violation-witness --witness witness.yml
// RUN: --memory-leak-check --no-reachable-memory-leak --no-abnormal-memory-leak --incremental-bmc --validate-violation-witness --witness witness.yml
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
