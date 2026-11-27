#include <assert.h>

void __VERIFIER_assume(_Bool);

struct inner
{
  int a;
  int b;
};

struct outer
{
  struct inner in;
  int t;
};

int main(void)
{
  struct outer o;
  struct inner x;
  __VERIFIER_assume(o.in.a == 1 && o.in.b == 2);
  /* The right-hand side is a struct-typed member expression, which is the
   * shape whose model value used to come back unresolved: the counterexample
   * has to report x as a materialised constant, not as an expression still
   * naming o.in (and not with the enclosing struct's fields). */
  x = o.in;
  assert(x.a + x.b != 3);
  return 0;
}
// Imported from ESBMC regression/esbmc/struct-member-model-value
// RUN: --no-incremental-bmc
// EXPECT: ^  x = \{ \.a=1, \.b=2 \}$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^  x = \{ \.a=1, \.b=2 \}$
// MODE: FUTURE
// Vereda: verdict FAILED
