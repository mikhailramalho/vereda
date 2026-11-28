// A variadic function reads back vector arguments (#305): a struct of two
// floats, which x86-64 passes as <2 x float>, a 16-byte vector of floats and
// a struct of four floats, which x86-64 passes as two <2 x float> in two SSE
// registers, or on the stack when two are not free. Each takes an SSE
// register while one is free, else the stack aligned to its type (the
// vector to 16). With n = 7 the doubles leave one SSE register on x86-64,
// and the vector follows f's slot on i386; with n = 8 they take them all.
// RUN: -m64
// RUN: -m32
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <stdarg.h>
extern float __VERIFIER_nondet_float(void);
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);

typedef float v4f __attribute__((vector_size(16)));
struct f2 { float a, b; };
struct f4 { float a, b, c, d; };

static int sum, k2, b2;
static struct f2 f2;
static v4f v2;
static struct f4 g2;

static void take(int n, ...) {
  va_list ap;
  va_start(ap, n);
  sum = 0;
  for (int i = 0; i < n; ++i)
    sum += (int)va_arg(ap, double);
  f2 = va_arg(ap, struct f2);
  k2 = va_arg(ap, int);
  v2 = va_arg(ap, v4f);
  g2 = va_arg(ap, struct f4);
  b2 = va_arg(ap, int);
  va_end(ap);
}

static void check(struct f2 f, int k, v4f v, struct f4 g) {
  __VERIFIER_assert(f2.a == f.a && f2.b == f.b && k2 == k && b2 == 9);
  __VERIFIER_assert(v2[0] == v[0] && v2[1] == v[1] && v2[2] == v[2] && v2[3] == v[3]);
  __VERIFIER_assert(g2.a == g.a && g2.b == g.b && g2.c == g.c && g2.d == g.d);
}

int main(void) {
  float x = __VERIFIER_nondet_float(), y = __VERIFIER_nondet_float();
  int k = __VERIFIER_nondet_int();
  __VERIFIER_assume(x == x && y == y);
  struct f2 f = {x, 2.0f};
  v4f v = {3.0f, y, 5.0f, x};
  struct f4 g = {y, 6.0f, x, 8.0f};
  take(0, f, k, v, g, 9);
  __VERIFIER_assert(sum == 0);
  check(f, k, v, g);
  take(7, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, f, k, v, g, 9);
  __VERIFIER_assert(sum == 7);
  check(f, k, v, g);
  take(8, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, f, k, v, g, 9);
  __VERIFIER_assert(sum == 8);
  check(f, k, v, g);
  return 0;
}
