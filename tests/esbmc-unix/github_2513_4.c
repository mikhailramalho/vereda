#include <stdlib.h>
#include <alloca.h>

extern int __VERIFIER_nondet_int(void);

int test_fun(int x, int y)
{
    int* x_ref = alloca(sizeof(int));
    int* y_ref = alloca(sizeof(int));
    int* c = alloca(sizeof(int));
    *x_ref = x;
    *y_ref = y;
    *c = 0;
    while (*x_ref >= 0) {
        *y_ref = 1;
        while (*x_ref > *y_ref && *y_ref < 1073741824) {
            *y_ref = 2*(*y_ref);
        }
        *x_ref = *x_ref - 1;
    }
    return *c;
}

int main() {
  return test_fun(__VERIFIER_nondet_int(),__VERIFIER_nondet_int());
}
// Imported from ESBMC regression/esbmc-unix/github_2513_4
// Needs: --force-realloc-success --state-hashing --add-symex-value-sets --interval-analysis --error-label ERROR --goto-unwind --unlimited-goto-unwind --k-induction --max-inductive-step 3
// RUN: --no-div-by-zero-check --force-malloc-success --force-realloc-success --state-hashing --add-symex-value-sets --no-align-check --k-step 2  --unlimited-k-steps --no-vla-size-check --64 --no-pointer-check --interval-analysis --no-bounds-check --error-label ERROR --goto-unwind --unlimited-goto-unwind --k-induction --max-inductive-step 3
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
