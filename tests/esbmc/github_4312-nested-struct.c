#include <assert.h>

// Regression for issue #4312-A nested case: zero-fill must descend
// into a nested struct, so the inner field is rendered for every
// witness even when the SMT model leaves it unconstrained.

struct inner
{
  int a;
  int b;
};

struct outer
{
  struct inner i;
  int c;
};

int main(void)
{
  struct outer o;
  if (o.i.a < -1 || o.i.a > 1)
    return 0;
  if (o.c < -1 || o.c > 1)
    return 0;
  // .i.b is unconstrained; the witness must still render it as 0.
  int absa = o.i.a < 0 ? -o.i.a : o.i.a;
  int absc = o.c < 0 ? -o.c : o.c;
  assert(absa + absc > 1);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_4312-nested-struct
// Needs: --all-witnesses --max-witnesses 4
// RUN: --all-witnesses --max-witnesses 4 --no-unwinding-assertions --unwind 1
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: \[Counterexamples - 4 witnesses\]
// EXPECT: ^[^\n]*Inputs : \[0\] = \{ \.i=\{ \.a=-?\d+, \.b=-?\d+ \}, \.c=-?\d+ \}$
// Vereda: verdict none
