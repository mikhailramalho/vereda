// EXPECT: ^VERIFICATION SUCCESSFUL$
// longjmp(b, 0) makes setjmp return 1 (C11 7.13.2.1p4).
#include <setjmp.h>
extern void __VERIFIER_assert(int);

jmp_buf b;

int main(void) {
  int r = setjmp(b);
  if (r == 0)
    longjmp(b, 0);
  __VERIFIER_assert(r == 1);
  return 0;
}
