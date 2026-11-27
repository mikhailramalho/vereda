// Codex adversarial-review case for the period-1 fixpoint detector.
// The program TERMINATES: global a is initialised to 1 and never
// re-written, so the validated input == 1 always hits the exit-branch
// in update(). A sound detector must NOT flag this as non-terminating.
//
// Without the soundness fix, the SMT formula was:
//   (caller_input == 1) AND NOT (a == 1 AND callee_input == 1)
// where caller_input and callee_input are distinct SMT symbols and a
// is a free integer. SAT (pick a = 2, callee_input = 7, caller_input
// = 1) → wrong "VERIFICATION FAILED".
//
// With the fix, the formula adds (a) substitution of callee_input by
// the caller's input symbol, and (b) the harvested initialiser a == 1,
// so the formula becomes:
//   (input == 1) AND a == 1 AND NOT (a == 1 AND input == 1)
// which is UNSAT — detector returns UNKNOWN, ranking/k-induction
// reports the actual verdict (here UNKNOWN, since the loop is not
// trivially provable terminating; the safety net is that we did not
// emit a wrong refutation).

extern int __VERIFIER_nondet_int(void);
extern void exit(int);

int a = 1;

int update(int input)
{
    if (a == 1 && input == 1)
        exit(0);
    return -2;
}

int main(void)
{
    int sink = 0;
    while (1) {
        int input;
        input = __VERIFIER_nondet_int();
        if (input != 1)
            return -2;
        sink = update(input);
    }
    return sink;
}
// Imported from ESBMC regression/termination/lasso_eca_unreachable_state
// Needs: --force-realloc-success --termination --max-inductive-step 3
// RUN: --ub-div-by-zero-check=false --force-malloc-success --force-realloc-success --no-align-check --k-step 2 --vla-size-check=false -m32 --ub-pointer-check=false --no-bounds-check --no-assertions --termination --max-inductive-step 3
// EXPECT: ^VERIFICATION (UNKNOWN|SUCCESSFUL)$
// MODE: FUTURE
// Vereda: verdict none
