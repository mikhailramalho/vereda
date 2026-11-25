// eca-shape benchmark where EVERY (state, input) hits a branch, so the
// fall-through `return -2` is unreachable and the period-1 fixpoint
// detector finds nothing. The program still runs forever: choosing
// input == 1 every iteration repeats the initial state.

extern int __VERIFIER_nondet_int(void);

int update(int input)
{
    // input in {1, 2} → some branch always fires.
    if (input == 1) return 10;
    if (input == 2) return 20;
    return -2;
}

int main(void)
{
    int sink = 0;
    while (1) {
        int input = __VERIFIER_nondet_int();
        if (input != 1 && input != 2)
            return -2;
        sink = update(input);
    }
    return sink;
}
// Imported from ESBMC regression/termination/lasso_eca_no_fallthrough
// Needs: --force-realloc-success --state-hashing --add-symex-value-sets --no-assertions --termination --max-inductive-step 3
// RUN: --no-div-by-zero-check --force-malloc-success --force-realloc-success --state-hashing --add-symex-value-sets --no-align-check --k-step 2  --no-vla-size-check --32 --no-pointer-check --no-bounds-check --no-assertions --termination --max-inductive-step 3
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
