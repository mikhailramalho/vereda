// Loop body has an `if (cond) return val;` arm that exits the loop
// early. Previously Shape B rejected the loop because the THEN arm
// did not end in a forward GOTO to a merge label. The inner-if-return
// extension treats the early-return arm as a path that exits the
// loop, contributing no ranking obligation; only the continue arm
// (path-condition: the IF's jump-guard) generates a ranking path.
//
// Lifted from termination-memory-alloca/b.03_assume-alloca.

extern int __VERIFIER_nondet_int(void);

int main(void)
{
    int x = __VERIFIER_nondet_int();
    int y = __VERIFIER_nondet_int();
    while (x > y && y <= 2147483647 - x) {
        if (x <= 0) {
            return y;
        }
        y = y + x;
    }
    return y;
}
// Imported from ESBMC regression/termination/ranking_inner_if_return
// Needs: --force-realloc-success --termination --max-inductive-step 3
// RUN: --no-div-by-zero-check --force-malloc-success --force-realloc-success --no-align-check --k-step 2 --no-vla-size-check --32 --no-pointer-check --no-bounds-check --no-assertions --termination --max-inductive-step 3
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
