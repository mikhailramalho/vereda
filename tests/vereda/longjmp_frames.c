// EXPECT: ^VERIFICATION SUCCESSFUL$
// A longjmp two frames down resumes main's setjmp, which returns its value;
// the code after the longjmp and after the calls does not run.
#include <setjmp.h>
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);
extern void __VERIFIER_assume(int);

jmp_buf b;
int trail;

void g(int v) {
  trail = 1;
  longjmp(b, v);
  trail = 2;
}

void f(int v) {
  g(v);
  trail = 3;
}

int main(void) {
  int v = __VERIFIER_nondet_int();
  __VERIFIER_assume(v != 0);
  int r = setjmp(b);
  if (r == 0) {
    f(v);
    __VERIFIER_assert(0);
  }
  __VERIFIER_assert(r == v && trail == 1);
  return 0;
}
