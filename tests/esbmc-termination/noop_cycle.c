/* No-op-cycle witness pattern. The loop body is `if (i != -5) i++;`
 * — for the state `i == -5`, the IF is not taken, the body has no
 * effect, and the loop iterates forever. Termination depends on
 * proving that some entry state cycles without state change.
 *
 * Vanilla k-induction IS cannot decide this: from an arbitrary
 * havoced `i < 0`, most starting states reach `i >= 0` within k
 * iterations, so IS reports SAT ("unable to prove non-termination").
 * Only `i == -5` traps. goto_terminationt's inject_noop_cycle_assumes
 * scans the loop body for IFs whose taken-branch reaches the back-
 * edge without crossing a state-modifying instruction; here, the IF
 * `if (i != -5) ...`'s "guard not satisfied" path (i.e. `i == -5`)
 * goes directly to the back-edge with nothing in between. The pass
 * injects ASSUME(!(i != -5)), i.e. ASSUME(i == -5), as an
 * inductive_step_instruction right before the post-havoc loop_head.
 * Pinned to that fixed-point state, IS proves the marker
 * unreachable → UNSAT → non-termination correctly reported. */

extern int __VERIFIER_nondet_int(void);

int main()
{
  int i = __VERIFIER_nondet_int();
  while (i < 0)
  {
    if (i != -5)
      i = i + 1;
  }
  return 0;
}
// Imported from ESBMC regression/termination/noop_cycle
// Needs: --force-realloc-success --termination --max-inductive-step 3
// RUN: --ub-div-by-zero-check=false --force-malloc-success --force-realloc-success --k-step 2 --vla-size-check=false -m32 --ub-pointer-check=false --no-assertions --termination --max-inductive-step 3
// EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict none
