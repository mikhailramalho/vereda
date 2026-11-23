// EXPECT: ^VERIFICATION SUCCESSFUL$
// The setjmp runs on one branch only, so the way back from the longjmp
// enters the code after it a second way; it goes through the function's
// restart block, which keeps the loop it makes reducible.
#include <setjmp.h>
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);

jmp_buf b;

int main(void) {
  int c = __VERIFIER_nondet_int();
  int r = 0;
  if (c)
    r = setjmp(b);
  if (r) {
    __VERIFIER_assert(r == 2 && c);
    return 0;
  }
  if (c)
    longjmp(b, 2);
  return 0;
}
