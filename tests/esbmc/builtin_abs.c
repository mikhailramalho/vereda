#include <assert.h>
#include <limits.h>

int __VERIFIER_nondet_int(void);
long __VERIFIER_nondet_long(void);

int main()
{
  assert(__builtin_abs(-3) == 3);
  assert(__builtin_labs(-3l) == 3);
  assert(__builtin_llabs(LLONG_MIN + 1) == LLONG_MAX);

  int x = __VERIFIER_nondet_int();
  __VERIFIER_assume(x != INT_MIN);
  assert(__builtin_abs(x) >= 0 && __builtin_abs(-x) == __builtin_abs(x));

  long y = __VERIFIER_nondet_long();
  __VERIFIER_assume(y != LONG_MIN);
  assert(__builtin_labs(y) == (y < 0 ? -y : y));
  return 0;
}
// Imported from ESBMC regression/esbmc/builtin_abs
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict UNKNOWN
