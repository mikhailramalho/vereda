#include <assert.h>
#include <math.h>

/* A tolerance the default 8 terms cannot meet: its sibling control runs the
   same program without the flag and must fail. */
int main()
{
  double y = log1p(0.125);
  assert(fabs(y - 0.11778303565638346) < 1e-15);
  return 0;
}
// Imported from ESBMC regression/floats/github_2865_high
// Needs: --fp-taylor-terms 12
// RUN: --fp-taylor-terms 12 --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
