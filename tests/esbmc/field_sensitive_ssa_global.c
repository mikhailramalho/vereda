#include <assert.h>
int __VERIFIER_nondet_int(void);
struct S { int a; int b; int *q; };
struct S g;
static struct S h = {7, 8, 0};
void set(struct S *p, int v) { p->a = v; p->b = v + 1; }
int bump(void) { static struct S c; c.a++; return c.a; }
int main(void)
{
  int x = __VERIFIER_nondet_int();
  assert(g.a == 0 && g.q == 0);
  g.q = &x;
  set(&g, 3);
  struct S *r = &g;
  assert(g.a == 3 && r->b == 4 && *r->q == x);
  h.b += g.a;
  assert(h.a == 7 && h.b == 11);
  struct S t = g;
  g = h;
  assert(t.a == 3 && g.a == 7 && g.q == 0);
  bump();
  assert(bump() == 2);
  return 0;
}
// Imported from ESBMC regression/esbmc/field_sensitive_ssa_global
// RUN: --no-incremental-bmc
// EXPECT: ^Field-sensitive SSA splits 4 struct object\(s\)$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^Field-sensitive SSA splits 4 struct object\(s\)$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
