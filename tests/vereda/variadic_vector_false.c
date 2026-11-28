// variadic_vector.c's call with n = 8, but the vector's last element is
// taken for its second: wrong unless x equals y.
// RUN: -m64
// RUN: -m32
// EXPECT: ^VERIFICATION FAILED$
#include <stdarg.h>
extern float __VERIFIER_nondet_float(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);

typedef float v4f __attribute__((vector_size(16)));
struct f2 { float a, b; };

static v4f v2;

static void take(int n, ...) {
  va_list ap;
  va_start(ap, n);
  for (int i = 0; i < n; ++i)
    (void)va_arg(ap, double);
  (void)va_arg(ap, struct f2);
  (void)va_arg(ap, int);
  v2 = va_arg(ap, v4f);
  va_end(ap);
}

int main(void) {
  float x = __VERIFIER_nondet_float(), y = __VERIFIER_nondet_float();
  __VERIFIER_assume(x == x && y == y);
  struct f2 f = {x, 2.0f};
  v4f v = {3.0f, y, 5.0f, x};
  take(8, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, f, 4, v, 9);
  __VERIFIER_assert(v2[3] == y);
  return 0;
}
