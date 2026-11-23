/* ./esbmc --cheri purecap --force-malloc-success assume.c --z3 */

#include <stdlib.h>
#include <assert.h>
#include <cheri/cheric.h>

int main() {
	int n = __VERIFIER_nondet_uint() % 1024; /* model arbitrary user input */
	char a[n+1];
	a[n] = 17; /* succeeds */
	__VERIFIER_assume(cheri_getlength(a) == n);
	a[n] = 23; /* succeeds - vacuous truth */
}
// Imported from ESBMC regression/cheri-128/02_assume-vacuous-purecap
// Needs: --cheri purecap --big-endian
// RUN: --cheri purecap --big-endian
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
