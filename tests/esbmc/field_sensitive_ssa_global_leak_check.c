#include <assert.h>

struct S
{
  int a;
  int b;
};

struct S g;

int main(void)
{
  struct S l = {2, 3};
  g.a = l.a;
  assert(g.a == 2 && g.b == 0 && l.b == 3);
  return 0;
}
// Imported from ESBMC regression/esbmc/field_sensitive_ssa_global_leak_check
// Needs: --memory-leak-check
// RUN: --memory-leak-check --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^Field-sensitive SSA splits 1 struct object\(s\)$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
