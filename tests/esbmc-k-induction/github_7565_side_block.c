/* `side` sits between the loop head and the back edge in program order but
 * only jumps out, so the jump to it never enters the loop and must not be
 * havoced: the program-order range over-approximates the loop body (#7565). */

#include <assert.h>

extern int __VERIFIER_nondet_int(void);

int main()
{
  int i = 0, s = 0;
  if (__VERIFIER_nondet_int())
    goto head;
  goto side;
head:
  i = 1;
  goto cont;
side:
  goto after;
cont:
  s = 1;
  if (__VERIFIER_nondet_int())
    goto head;
after:
  assert(i == 0 || i == 1);
  return 0;
}
// Imported from ESBMC regression/k-induction/github_7565_side_block
// Needs: --k-induction
// RUN: --k-induction --max-k-step 4
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: ^Solution found by the inductive step.*$
// Vereda: verdict none
