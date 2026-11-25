// Large-copy lambdas keep pointer provenance after byte and float reads.
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <string.h>
extern unsigned __VERIFIER_nondet_uint(void);
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
struct Big { int *pointer; double value; unsigned char padding[1024]; };
int main(void) {
  int x = 11;
  struct Big source = {&x, 3.5, {0}}, a, b;
  memcpy(&a, &source, sizeof(a));
  memcpy(&b, &source, sizeof(b));
  struct Big *selected = __VERIFIER_nondet_int() ? &a : &b;
  unsigned index = __VERIFIER_nondet_uint();
  __VERIFIER_assume(index < sizeof(struct Big));
  volatile unsigned char byte = ((unsigned char *)selected)[index];
  volatile double number = selected->value;
  __VERIFIER_assert(number == 3.5);
  __VERIFIER_assert(selected->pointer == &x);
  *selected->pointer = 19;
  __VERIFIER_assert(x == 19);
}
