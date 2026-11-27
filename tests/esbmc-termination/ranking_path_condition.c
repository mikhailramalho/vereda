/* Path-condition seeding for the ranking checker.
 *
 * The decrease obligation `x + c > 0 /\ (x - c) + (c + 1) >= x + c` of
 * this loop simplifies to `c > 1`, which is not derivable from a
 * constant-init seed (`c` is nondet). The fact that holds at the loop
 * head is `c >= 2`, supplied by the pre-loop `if (c >= 2) ... ` whose
 * fall-through edge leads into the loop. collect_seeds analyses the IF
 * via a bounded forward reachability check from each branch to the loop
 * head; since only the fall-through reaches the head, the seed atom is
 * the negation of the IF's GOTO guard, i.e. `c >= 2`. With that atom
 * inductive under the body (c' = c + 1, still >= 2), the decrease
 * obligation discharges.
 *
 * Expected verdict: VERIFICATION SUCCESSFUL. */

extern int __VERIFIER_nondet_int(void);

int main()
{
  int x = __VERIFIER_nondet_int();
  int c = __VERIFIER_nondet_int();
  if (c >= 2)
  {
    while (x + c >= 0)
    {
      x = x - c;
      c = c + 1;
    }
  }
  return 0;
}
// Imported from ESBMC regression/termination/ranking_path_condition
// Needs: --force-realloc-success --termination --max-inductive-step 3
// RUN: --ub-div-by-zero-check=false --force-malloc-success --force-realloc-success --k-step 2 --vla-size-check=false -m32 --ub-pointer-check=false --no-assertions --termination --max-inductive-step 3
// EXPECT: ^Ranking function shows all executions terminate
// EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none
