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
	if (__VERIFIER_nondet_bool()) exit(1);
}

void f() {
	*g = (int *) malloc(sizeof(int));
	atexit(free_g2);
	h();
}


int main() {
	g = (int **) malloc(sizeof(int *));
	atexit(free_g1);
	if (__VERIFIER_nondet_bool()) exit(1);
	f();
	return 0;
}

// Imported from ESBMC regression/esbmc/atexit-4
// Needs: --memory-leak-check
// RUN: --memory-leak-check --force-malloc-success --no-simplify --unwind 3
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
