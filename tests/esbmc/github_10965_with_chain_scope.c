#include <assert.h>
int __VERIFIER_nondet_int(void);

struct S { int *p; int tag; };

int main()
{
  int a = 1, b = 1;
  int *q = __VERIFIER_nondet_int() ? &a : &b;
  struct S r;
  {
    struct S t;
    t.p = q;
    t.tag = 0;
    r = t;
  }
  struct S r2 = r;
  assert(*r2.p == 1);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10965_with_chain_scope
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
