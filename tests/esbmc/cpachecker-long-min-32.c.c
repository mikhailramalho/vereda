#include <limits.h>
#include <assert.h>

long __VERIFIER_nondet_long(void);

void __VERIFIER_error(void);
void reach_error() { __VERIFIER_error(); }
void nope() { ERROR: reach_error(); }

int main()
{
	long x = __VERIFIER_nondet_long();
	if (x > LONG_MIN)
		return 0;
	if (x == LONG_MIN)
		nope();
	return 0;
}
// Imported from ESBMC regression/esbmc/cpachecker-long-min-32.c
// RUN: -m32 --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: x = -2147483648
