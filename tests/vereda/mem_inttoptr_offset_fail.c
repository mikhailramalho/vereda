// An address past one past the end of an object is not in it
// (mem_inttoptr_offset): it may be in the next object whose address the
// program exposed, here b, which the layout may put right after a's gap.
// RUN: --no-pointer-check
// EXPECT: ^VERIFICATION FAILED$
#include <stdint.h>
extern void __VERIFIER_assert(int);
int a[2], b[2];
uintptr_t saved;
int main(void) {
  saved = (uintptr_t)b;
  *(int *)((uintptr_t)a + 2 * sizeof a) = 1;
  __VERIFIER_assert(b[0] == 0);
  return 0;
}
