#include <math.h>

int main()
{
  double x = __VERIFIER_nondet_double();
  double y = __VERIFIER_nondet_double();

  __VERIFIER_assert(isless(x, y) == (x < y)); // isless agrees with <
  __VERIFIER_assert(isgreater(x, y) == (x > y)); // isgreater agrees with >
  __VERIFIER_assert(islessequal(x, y) == (x <= y)); // islessequal agrees with <=
  __VERIFIER_assert(
    isgreaterequal(x, y) == (x >= y)); // isgreaterequal agrees with >=
  __VERIFIER_assert(
    islessgreater(x, y) == (x < y || x > y)
); // islessgreater agrees with < or >
  __VERIFIER_assert(
    isunordered(x, y) == !(x < y || x > y || x == y)
); // isunordered is the absence of an ordering

  __VERIFIER_assert(
    isless(x, y) + isgreater(x, y) + (x == y) + isunordered(x, y) == 1
); // exactly one ordering relation holds

  return 0;
}
// Imported from ESBMC regression/esbmc/github_1229
// EXPECT: ^VERIFICATION SUCCESSFUL$
