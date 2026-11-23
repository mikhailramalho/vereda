// EXPECT: ^VERIFICATION UNKNOWN: unsupported: longjmp back past a value its setjmp needs in main$
// The way back through the restart block skips the code before the setjmp,
// here the VLA's allocation, whose address is used after it: UNKNOWN, not a
// guess.
#include <setjmp.h>
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);

jmp_buf b;

int main(void) {
  int n = __VERIFIER_nondet_int();
  __VERIFIER_assume(n > 0 && n < 4);
  int a[n];
  a[0] = 5;
  int c = __VERIFIER_nondet_int(), r = 0;
  if (c)
    r = setjmp(b);
  if (r) {
    __VERIFIER_assert(a[0] == 5);
    return 0;
  }
  if (c)
    longjmp(b, 1);
  return 0;
}
