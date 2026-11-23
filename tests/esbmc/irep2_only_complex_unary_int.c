#include <assert.h>

/* The integral element type reaches the same unary arm as
   irep2_only_complex_unary's double one. Split from it because both element
   types in one formula take z3 past the Windows job's limit. */

int __VERIFIER_nondet_int(void);

int main(void)
{
  int _Complex a;
  __real__ a = __VERIFIER_nondet_int();
  __imag__ a = __VERIFIER_nondet_int();
  int ar = __real__ a, ai = __imag__ a;
  __VERIFIER_assume(ar >= 1 && ar <= 4 && ai >= 1 && ai <= 4);

  int _Complex m = -a;
  assert(__real__ m == -ar && __imag__ m == -ai);
  int _Complex k = ~a;
  assert(__real__ k == ar && __imag__ k == -ai);

  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_complex_unary_int
// Needs: --clang-c-irep2-adjust-only
// RUN: --clang-c-irep2-adjust-only
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
