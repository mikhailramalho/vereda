#include <assert.h>
#include <float.h>
#include <math.h>

int main(void)
{
  assert(nextafterl(1.0L, 2.0L) == 1.0L);
  assert(nextafterl(1.0L, 0.0L) == 1.0L - LDBL_EPSILON);
  assert(nexttoward(1.0, 0.0L) == 1.0 - DBL_EPSILON);
  assert(nexttowardf(1.0f, 2.0L) == 1.0f);
  return 0;
}
// Imported from ESBMC regression/esbmc/nextafterl_model_fail
// RUN: --multi-property
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: nextafterl\(1.0L, 2.0L\) == 1.0L$
// EXPECT: nextafterl\(1.0L, 0.0L\) == 1.0L - LDBL_EPSILON$
// EXPECT: nexttoward\(1.0, 0.0L\) == 1.0 - DBL_EPSILON$
// EXPECT: nexttowardf\(1.0f, 2.0L\) == 1.0f$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict FAILED
