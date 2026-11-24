// EXPECT: ^VERIFICATION SUCCESSFUL$
// The setjmp is in a loop the longjmp is not, and the loop can end without
// running it: the way back enters the loop at a second block. ir turns the
// cycle into a loop with one head (FixIrreducible), so the jump is walked.
// With --unwind, both Vereda and ESBMC 8.3 need 3 to give SUCCESSFUL.
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
