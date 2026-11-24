// Structs clang returns as first-class aggregates on x86-64, each holding a
// pointer next to another field: a double ({double, ptr}), a bool and a
// char packed into an i64 ({i64, ptr}), a nested struct ({ptr, i64}), three
// floats ({<2 x float>, float}), one picked by a nondet select, one copied
// from a global, and one whose address escapes before it is returned.
// RUN: -m64 --unwind 5
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <stdbool.h>
extern void __VERIFIER_assert(int);
extern int __VERIFIER_nondet_int(void);

struct S { int *p; int n; };
struct D { double a; int *p; };
struct B { bool b; char c; int *p; };
struct N { struct { int *p; } in; long n; };
struct F { float a, b, c; };

static void touch(struct S *s) { s->n++; }
static struct S escaped(int *p) { struct S s = {p, 1}; touch(&s); return s; }
static struct D md(int *p, double a) { struct D d = {a, p}; return d; }
static struct B mb(int *p, bool b) { struct B r = {b, 'x', p}; return r; }
static struct N mn(int *p) { struct N r = {{p}, 7}; return r; }
static struct F mf(float x) { struct F f = {x, x + 1, x + 2}; return f; }
static struct S pick(int c, int *a, int *b) {
  struct S x = {a, 1}, y = {b, 2};
  return c ? x : y;
}
struct S g;
static struct S fromg(void) { return g; }

int main(void) {
  int x = 1, y = 5;
  struct S s = escaped(&x);
  struct D d = md(&y, 2.5);
  struct B b = mb(&x, true);
  struct N n = mn(&y);
  struct F f = mf(1.0f);
  int c = __VERIFIER_nondet_int();
  struct S q = pick(c, &x, &y);
  g.p = &y;
  g.n = 3;
  struct S h = fromg();
  __VERIFIER_assert(*s.p == 1 && s.n == 2);
  __VERIFIER_assert(*d.p == 5 && d.a == 2.5);
  __VERIFIER_assert(b.b && b.c == 'x' && *b.p == 1);
  __VERIFIER_assert(*n.in.p == 5 && n.n == 7);
  __VERIFIER_assert(f.c == 3.0f);
  __VERIFIER_assert(*q.p == (c ? 1 : 5) && q.n == (c ? 1 : 2));
  __VERIFIER_assert(*h.p + h.n == 8);
  return 0;
}
