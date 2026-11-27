// A global only main uses keeps the value it had at a longjmp back to
// main's setjmp: it becomes a local, but a global's value, unlike an
// automatic one's, is not indeterminate after the jump (C17 7.13.2.1).
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <setjmp.h>
extern void __VERIFIER_assert(int);
int g;
int main(void) {
  jmp_buf b;
  if (setjmp(b) == 0) {
    g = 1;
    longjmp(b, 1);
  }
  __VERIFIER_assert(g == 1);
  return 0;
}
