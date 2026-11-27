#include <assert.h>

/* _Complex rather than <complex.h>'s `complex`: MSVC ships no C99 complex
   header, so the macro spelling does not parse there. */

double __VERIFIER_nondet_double(void);

int main(void)
{
  double _Complex z;
  __real__ z = __VERIFIER_nondet_double();
  __imag__ z = __VERIFIER_nondet_double();
  __VERIFIER_assume(__imag__ z > 3.0 && __imag__ z < 4.0);

  /* Conjugation flips the sign of the imaginary part, so this is violated --
     a property broken through the lowering stays reportable on the component
     the lowering built. */
  double _Complex c = ~z;
  assert(__imag__ c == __imag__ z);

  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_complex_unary_fail
// EXPECT: ^VERIFICATION FAILED$
