
/* keep in sync with ../nextafter-spec/main.c */

#include <float.h>
#include <math.h>
#include <errno.h>

#undef errno
extern int errno;

int main()
{
  float x = __VERIFIER_nondet_float();
  float y = __VERIFIER_nondet_float();
  float z = nextafterf(x, y);

  /* any NaN argument <-> NaN result */
  assert((isnan(x) || isnan(y)) == (isnan(z) != 0));

  __VERIFIER_assume(!isnan(z));
  float a = __VERIFIER_nondet_float();

  if(isless(x, y))
  { /* there is no double 'a' between x and z */
    assert(isnan(a) || islessequal(a, x) || islessequal(z, a));
    __VERIFIER_assume(islessequal(z, a)); /* assume z <= a */
  }
  if(isgreater(x, y))
  { /* there is no double 'a' between x and z */
    assert(isnan(a) || islessequal(a, z) || islessequal(x, a));
    __VERIFIER_assume(islessequal(a, z)); /* assume z >= a */
  }
  if(x == y)
    assert(x == z);
  if(isfinite(x))
  { /* |z| not finite -> |a| = inf and they have the same sign */
    assert(isfinite(z) || (isinf(a) && isinf(a) == isinf(z)));
  }
  if(isfinite(x) && !isfinite(z))
  {
    /* This is part of nextafter's behavior, but w/o a model of
     * feraiseexcept() and fetestexcept() we cannot prove it, yet.
    assert(fetestexcept(FE_OVERFLOW));
     */
    assert(errno == ERANGE);
  }
  if(islessgreater(x, y) && isfinite(z) && !isnormal(z))
  {
    /* This is part of nextafter's behavior, but w/o a model of
     * feraiseexcept() and fetestexcept() we cannot prove it, yet.
    assert(fetestexcept(FE_UNDERFLOW));
     */
    assert(errno == ERANGE);
  }
  if(isinf(x) && x != y)
    assert(fabsf(z) == FLT_MAX);

  /* This is part of nextafter's behavior, but w/o a model of
   * feraiseexcept() and fetestexcept() we cannot prove it, yet.
  assert(!fetestexcept(FE_UNDERFLOW) || !fetestexcept(FE_OVERFLOW));
   */
}
// Imported from ESBMC regression/floats-regression/nextafterf-spec
// Needs: --interval-analysis
// RUN: --interval-analysis
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
