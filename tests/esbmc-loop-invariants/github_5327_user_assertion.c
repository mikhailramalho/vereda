/* Companion to function_contract/github_5327: scoping the vacuity probe to
 * user-facing claims must not stop it firing on a user assertion. The two
 * assumes contradict, so the assert below is discharged on a dead path. */
#include <assert.h>

extern int __VERIFIER_nondet_int(void);

int main(void)
{
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assume(x > 0);
  __VERIFIER_assume(x < 0);

  int i = 0;
  __ESBMC_loop_invariant(i <= 3);
  while (i < 3)
  {
    i = i + 1;
  }

  assert(x == 42);
  return 0;
}
// Imported from ESBMC regression/loop-invariants/github_5327_user_assertion
// Needs: --loop-invariant-check
// RUN: --loop-invariant-check --unwind 5
// MODE: FUTURE
// EXPECT: ^WARNING: Vacuous discharge: claim 'assertion
// EXPECT: ^VERIFICATION UNKNOWN$
// Vereda: verdict none
