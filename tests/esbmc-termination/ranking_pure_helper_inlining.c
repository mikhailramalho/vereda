// Loop body has two FUNCTION_CALLs to a pure helper `foo` that
// decrements a global scalar `x`. Without inlining, the certifier
// rejects the loop because FUNCTION_CALL terminates the recognized
// straight-line span. With targeted pure-helper inlining, `foo()` is
// expanded into its body (`x = x - 1;`) at each call site, the
// span becomes `x = x - 1` regardless of the inner branch, and the
// measure `x` strictly decreases.
//
// Lifted from loops/trex02-1.c.

extern int __VERIFIER_nondet_int(void);

int x;

void foo(void)
{
    x--;
}

int main(void)
{
    x = __VERIFIER_nondet_int();
    while (x > 0) {
        int c = __VERIFIER_nondet_int();
        if (c)
            foo();
        else
            foo();
    }
    return 0;
}
// Imported from ESBMC regression/termination/ranking_pure_helper_inlining
// Needs: --force-realloc-success --state-hashing --add-symex-value-sets --k-step 2 --no-assertions --termination --max-inductive-step 3 REQUIRES arch32
// RUN: --no-div-by-zero-check --force-malloc-success --force-realloc-success --state-hashing --add-symex-value-sets --no-align-check --k-step 2 --floatbv --no-vla-size-check --32 --no-pointer-check --no-bounds-check --no-assertions --termination --max-inductive-step 3
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
