#include <assert.h>
typedef struct
{
  _Bool (*a)();
} c;
void *e;
c f[1];
c *g = f;
_Bool b()
{
  return 1;
}
void d();
void main()
{
  int i = __VERIFIER_nondet_int();
  f[0] = (c){b};
  __VERIFIER_assume(i >= 0 && i < 1);
  if (g[i].a())
    e = d;
  assert(e);
}
// Imported from ESBMC regression/esbmc/github_2292_4
// Needs: --interval-analysis --interval-analysis-simplify
// RUN: --interval-analysis --interval-analysis-simplify --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
