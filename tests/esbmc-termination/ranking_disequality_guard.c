// Termination via a `!=` countdown guard. Mirrors the Cairo shape from
// termination-crafted: an outer `if (x > 0)` makes x strictly positive
// at loop entry, then `while (x != 0) x = x - 1;` decrements to zero.
// The certifier sees the `!=` and emits two direction candidates;
// the `(x - 0)` candidate with the relaxed invariant `x >= 0`
// (weakened from the path-cond seed `x > 0`) discharges.

extern int __VERIFIER_nondet_int(void);

int main(void)
{
    int x = __VERIFIER_nondet_int();
    if (x > 0) {
        while (x != 0) {
            x = x - 1;
        }
    }
    return 0;
}
// Imported from ESBMC regression/termination/ranking_disequality_guard
// Needs: --force-realloc-success --termination --max-inductive-step 3
// RUN: --ub-div-by-zero-check=false --force-malloc-success --force-realloc-success --no-align-check --k-step 2 --vla-size-check=false -m32 --ub-pointer-check=false --no-bounds-check --no-assertions --termination --max-inductive-step 3
// EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none
