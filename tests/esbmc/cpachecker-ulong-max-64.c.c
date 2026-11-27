#include <limits.h>
#include <assert.h>

unsigned long __VERIFIER_nondet_ulong(void);

void __VERIFIER_error(void);
void reach_error() { __VERIFIER_error(); }
void nope() { ERROR: reach_error(); }

int main()
{
	unsigned long x = __VERIFIER_nondet_ulong();
	if (x < ULONG_MAX)
		return 0;
	if (x == ULONG_MAX)
		nope();
	return 0;
}
// Imported from ESBMC regression/esbmc/cpachecker-ulong-max-64.c
// RUN: -m64
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: x = 0xFFFFFFFFFFFFFFFF
// Vereda: unmet EXPECT: x = 0xFFFFFFFFFFFFFFFF
// MODE: FUTURE
// Vereda: verdict FAILED
