// github #10965
#include <assert.h>
struct in { int *p; };
struct out { struct in a; };
_Bool __VERIFIER_nondet_bool(void);
int main(void)
{
  int x = 7, y = 7, z = 5;
  int *q = __VERIFIER_nondet_bool() ? &x : &y;
  struct out t, tmp, u;
  t.a.p = q;
  tmp = t;
  t.a.p = &z;
  u.a.p = &z;
  struct out *pp = __VERIFIER_nondet_bool() ? &t : &u;
  *pp = tmp;
  assert(*t.a.p == 5); /* fails when pp == &t */
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10965_guarded_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
