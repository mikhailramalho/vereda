#include <stdbool.h>

bool eq(int a, int b)
{
  return a == b;
}

int main()
{
  int var;
  __VERIFIER_assert(__ESBMC_forall(&var, eq(var, 6))); // forall-eq-6
}
// Imported from ESBMC regression/z3/github_6100_fail
// RUN: --solver z3 --incremental-bmc
// MODE: THOROUGH
// EXPECT: ^VERIFICATION FAILED$
