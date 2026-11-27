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
// Needs: --force-realloc-success --interval-analysis --k-induction --max-inductive-step 3
// RUN: --ub-div-by-zero-check=false --force-malloc-success --force-realloc-success --k-step 2 --unlimited-k-steps --vla-size-check=false -m64 --ub-pointer-check=false --interval-analysis --error-label ERROR --k-induction --max-inductive-step 3
// EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none
