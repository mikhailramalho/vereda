#include <assert.h>
#include <stdint.h>

int main()
{
  int d = __VERIFIER_nondet_int();
  int c = __VERIFIER_nondet_int();
  char e = __VERIFIER_nondet_char();

  // Cast on one side breaks canonical cancellation form
  assert(((d + c) == (d + (int)e)) ? (c == (int)e) : 1);

  return 0;
}

// Imported from ESBMC regression/esbmc/simplifier15
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
