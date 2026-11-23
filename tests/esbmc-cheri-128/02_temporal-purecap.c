/* ./esbmc --cheri purecap --force-malloc-success temporal.c */

#include <stdlib.h>
#include <assert.h>
#include <cheri/cheric.h>

int main() {
	int n = __VERIFIER_nondet_uint() % 1024; /* model arbitrary user input */
	char *__capability a = malloc_c(1+n); /* CHERI-C API */
	assert(cheri_gettag(a));              /* CHERI-C API */
	*a = 5;
	free_c(a);                            /* CHERI-C API */
	*a = 5; /* use-after-free bug */
}
// Imported from ESBMC regression/cheri-128/02_temporal-purecap
// Needs: --cheri purecap --big-endian
// RUN: --cheri purecap --big-endian --force-malloc-success
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
