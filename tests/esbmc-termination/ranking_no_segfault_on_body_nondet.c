// Regression for a crash in the ranking certifier when the body
// assigns a NONDET value (sideeffect2t) to a variable that the
// candidate measure depends on. The post-state measure
// `m_prime = apply_body(m, p.assigns)` then contained the
// sideeffect, and feeding it to the SMT layer triggered a
// SIGSEGV in `smt_convt::convert_ast` (the hash-cache lookup
// crashed on the embedded null operand inside the sideeffect
// expression).
//
// The fix filters sideeffect-containing post-state measures
// before the SMT discharge — same conservative behaviour as
// the `contains_sideeffect` check on path-condition atoms.
//
// Lifted from termination-memory-alloca/CookSeeZuleger-2013TACAS-Fig3-alloca-2.

#include <stdlib.h>
#include <alloca.h>
extern int __VERIFIER_nondet_int(void);

int main(void)
{
    int *x = (int *)alloca(sizeof(int));
    int *y = (int *)alloca(sizeof(int));
    *x = __VERIFIER_nondet_int();
    *y = __VERIFIER_nondet_int();
    while (*x > 0 && *y > 0) {
        if (__VERIFIER_nondet_int()) {
            *x = *x - 1;
        } else {
            *x = __VERIFIER_nondet_int();
            *y = *y - 1;
        }
    }
    return 0;
}
// Imported from ESBMC regression/termination/ranking_no_segfault_on_body_nondet
// Needs: --force-realloc-success --state-hashing --add-symex-value-sets --k-step 2 --no-assertions --termination --max-inductive-step 3 REQUIRES arch32
// RUN: --no-div-by-zero-check --force-malloc-success --force-realloc-success --state-hashing --add-symex-value-sets --no-align-check --k-step 2 --floatbv --no-vla-size-check --32 --no-pointer-check --no-bounds-check --no-assertions --termination --max-inductive-step 3
// MODE: FUTURE
// EXPECT: ^VERIFICATION (UNKNOWN|SUCCESSFUL|FAILED)$
// Vereda: verdict none
