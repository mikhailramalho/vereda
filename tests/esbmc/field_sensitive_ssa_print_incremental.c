#include <assert.h>

struct pair
{
  int q;
  int r;
};

struct pair f(int n)
{
  struct pair p;
  p.q = n / 2;
  p.r = n % 2;
  return p;
}

int main(void)
{
  struct pair d = f(7);
  assert(d.q == 3);
  assert(d.r == 1);
  return 0;
}
// Imported from ESBMC regression/esbmc/field_sensitive_ssa_print_incremental
// Needs: --field-sensitive-ssa
// RUN: --field-sensitive-ssa --incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
