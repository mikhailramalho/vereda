#include <stdlib.h>

extern _Bool __VERIFIER_nondet_bool();

/* simple regression test for atexit */

int **g = NULL;

void free_g1() {
        free(g);
        g = NULL;
}

void free_g2() {
        if (g != NULL)
                free(*g);
}

void h() {
        if (__VERIFIER_nondet_bool()) exit(1); // memory leak
}

void f() {
        *g = (int *) malloc(sizeof(int));
        atexit(free_g1);
        h();
}


int main() {
        g = (int **) malloc(sizeof(int *));
        atexit(free_g2);
//      if (__VERIFIER_nondet_bool()) exit(1);
        f();
        free(*g);
        free(g);
        g = NULL;
        return 0;
}
// Imported from ESBMC regression/esbmc/github_2335_3
// Needs: --memory-leak-check --no-reachable-memory-leak --no-abnormal-memory-leak --malloc-zero-is-null
// RUN: --ub-div-by-zero-check=false --force-malloc-success --no-align-check --k-step 2 --unlimited-k-steps --vla-size-check=false --memory-leak-check --no-reachable-memory-leak --no-abnormal-memory-leak --malloc-zero-is-null
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
