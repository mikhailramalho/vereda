#include <assert.h>

int __VERIFIER_nondet_int(void);

struct S
{
  int n;
  int x;
};

int main(void)
{
  struct S s;
  s.n = 3;
  s.x = __VERIFIER_nondet_int();
  struct S t = s;
  int *p = &t.x;
  int i = 0;
  while (i < t.n)
    i++;
  assert(i == 3);
  assert(*p == s.x);
  return 0;
}
// Imported from ESBMC regression/esbmc/field_sensitive_ssa_copy_pinned
// RUN: --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^Field-sensitive SSA splits 1 struct object\(s\)$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict SUCCESSFUL
