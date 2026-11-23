// EXPECT: ^VERIFICATION UNKNOWN: unsupported: irreducible control flow in main$
// The setjmp is in a loop the longjmp is not, and the loop can end without
// running it: the way back enters the loop at a second block, which the
// walker does not walk. UNKNOWN, not a guess.
#include <setjmp.h>
extern void __VERIFIER_assert(int);

jmp_buf b;

int main(void) {
  int r = 0;
  for (int i = 0; i < 2; i++) {
    r = setjmp(b);
    if (r)
      break;
  }
  if (!r)
    longjmp(b, 1);
  __VERIFIER_assert(r == 1);
  return 0;
}
