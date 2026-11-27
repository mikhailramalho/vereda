// github #10965
#include <assert.h>
struct in { int *p; };
struct out { struct in a; };
_Bool __VERIFIER_nondet_bool(void);
int main(void)
{
  int x = 5, y = 5, z = 5;
  int *q = __VERIFIER_nondet_bool() ? &x : &y;
  struct out tmp, r;
  {
    struct out t;
    t.a.p = q;
    tmp = t;
  }
  r.a.p = &z;
  if (__VERIFIER_nondet_bool())
    r = tmp;
  assert(*r.a.p == 5); /* holds */
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10965_phi_scope
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
