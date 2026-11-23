// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^Violated property:\n  file \S*longjmp_frames_false\.c line 26 column 3 function main\n  __VERIFIER_assert$
// setjmp returns whatever value the longjmp two frames down passes it.
#include <setjmp.h>
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);
extern void __VERIFIER_assume(int);

jmp_buf b;

void g(int v) {
  longjmp(b, v);
}

void f(int v) {
  g(v);
}

int main(void) {
  int v = __VERIFIER_nondet_int();
  __VERIFIER_assume(v != 0);
  int r = setjmp(b);
  if (r == 0)
    f(v);
  __VERIFIER_assert(r != 0);
  __VERIFIER_assert(r != 7);
  return 0;
}
