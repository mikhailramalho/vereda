// From sv-benchmarks c/loop-invgen/SpamAssassin-loop.c, which --gcse reported
// as FAILED while every other configuration agrees it is safe.
//
// `i < len` reaches GCSE as a candidate through the __VERIFIER_assert call
// arguments, and the loop guards then become replacement sites. Since
// goto_k_induction lifts a loop's entry condition straight out of its guard, a
// symbol standing in for `i < len` ends up inside `assume(...)` while the
// assignment defining it stays below, past the havoc of `i` -- so the assume
// constrains the value the symbol held before the havoc and says nothing about
// the fresh `i`.
//
// Bounded k keeps this fast: the false alarm lands at k=3, so UNKNOWN here
// means "did not claim a bug", which is the property under test.
extern int __VERIFIER_nondet_int(void);
extern void abort(void);
void reach_error(void) { abort(); }
void __VERIFIER_assert(int cond) { if (!cond) reach_error(); }

int main()
{
    int len;
    int i;
    int j;

    int bufsize;
    bufsize = __VERIFIER_nondet_int();
    if (bufsize < 0) return 0; // avoid overflows for too negative values
    len = __VERIFIER_nondet_int();
    int limit = bufsize - 4;


    for (i = 0; i < len; ) {
        for (j = 0; i < len && j < limit; ){
            if (i + 1 < len){ 
                __VERIFIER_assert(i+1<len);
                __VERIFIER_assert(0<=i);
                if( __VERIFIER_nondet_int() ) goto ELSE;
                __VERIFIER_assert(i<len);
                __VERIFIER_assert(0<=i);
                __VERIFIER_assert(j<bufsize);
                __VERIFIER_assert(0<=j);

                j++;
                i++;
                __VERIFIER_assert(i<len);
                __VERIFIER_assert(0<=i);
                __VERIFIER_assert(j<bufsize);
                __VERIFIER_assert(0<=j);

                j++;
                i++;
                __VERIFIER_assert(j<bufsize);
                __VERIFIER_assert(0<=j);
                j++;
            } else {
ELSE:
                __VERIFIER_assert(i<len);
                __VERIFIER_assert(0<=i);
                __VERIFIER_assert(j<bufsize);
                __VERIFIER_assert(0<=j);
                j++;
                i++;
            }
        }
    }
    return 0;
}
// Imported from ESBMC regression/esbmc/gcse_kinduction_loop_guard
// Needs: --sv-comp --state-hashing --add-symex-value-sets --interval-analysis --goto-unwind --unlimited-goto-unwind --k-induction --max-inductive-step 3 --gcse
// RUN: --sv-comp --no-div-by-zero-check --force-malloc-success --state-hashing --add-symex-value-sets --no-align-check --k-step 2 --floatbv --no-vla-size-check --32 --no-pointer-check --interval-analysis --no-bounds-check --goto-unwind --unlimited-goto-unwind --k-induction --max-inductive-step 3 --max-k-step 6 --gcse
// MODE: FUTURE
// EXPECT: ^VERIFICATION UNKNOWN$
// Vereda: verdict none
