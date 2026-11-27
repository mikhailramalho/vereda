// Two-counter loop where no single measure decreases on every path.
// Guard `x < y && z < INT_MAX` yields candidates (y-x, 1) and
// (INT_MAX-z, 1). Path A (x < z) increments x → (y-x) decreases,
// (INT_MAX-z) unchanged. Path B (x >= z) increments z → (y-x)
// unchanged, (INT_MAX-z) decreases. Neither candidate decreases on
// every path individually, but the lex tuple ((y-x), (INT_MAX-z))
// decreases lex on both paths — Path A strictly via the first
// component, Path B equal-then-strict via the second.
//
// Lifted from termination-restricted-15/c.03.c.

extern int __VERIFIER_nondet_int(void);

int main(void)
{
    int x = __VERIFIER_nondet_int();
    int y = __VERIFIER_nondet_int();
    int z = __VERIFIER_nondet_int();
    while (x < y && z < 2147483647) {
        if (x < z) {
            x = x + 1;
        } else {
            z = z + 1;
        }
    }
    return 0;
}
// Imported from ESBMC regression/termination/ranking_lexicographic_2d
// Needs: --force-realloc-success --termination --max-inductive-step 3
// RUN: --ub-div-by-zero-check=false --force-malloc-success --force-realloc-success --k-step 2 --vla-size-check=false -m32 --ub-pointer-check=false --no-assertions --termination --max-inductive-step 3
// EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none
