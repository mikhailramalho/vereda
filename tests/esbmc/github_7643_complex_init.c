// A two-element brace initialiser gives a complex object its real and
// imaginary parts, a clang extension (github #7643).
#include <assert.h>

int __VERIFIER_nondet_int(void);

_Complex int gi = {1, 2};

int main(void)
{
  _Complex float fl = {1.5, 2};
  assert(__real__ gi == 1 && __imag__ gi == 2);
  assert(__real__ fl == 1.5f && __imag__ fl == 2.0f);

  int re = __VERIFIER_nondet_int(), im = __VERIFIER_nondet_int();
  _Complex int z = {re, im};
  assert(__real__ z == re && __imag__ z == im);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7643_complex_init
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
