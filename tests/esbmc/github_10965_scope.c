// github #10965
#include <assert.h>
struct in { int *p; };
struct out { struct in a; };
_Bool __VERIFIER_nondet_bool(void);

int main(void)
{
  int x = 5, y = 5;
  int *q = __VERIFIER_nondet_bool() ? &x : &y;
  struct out tmp;
  {
    struct out t;
    t.a.p = q;
    tmp = t;
  }
  struct out r = tmp;
  assert(*r.a.p == 5);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10965_scope
// EXPECT: ^VERIFICATION SUCCESSFUL$
