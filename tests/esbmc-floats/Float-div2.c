#include <assert.h>
#include <math.h>

float __VERIFIER_nondet_float (void);

int main (void) {
	float f = __VERIFIER_nondet_float();
        // Everything in this range hits the bug
	__VERIFIER_assume(f < +INFINITY);
	__VERIFIER_assume(f >= 0x1.0p+107f);
	__VERIFIER_assume(f > 0.0f);

	float g = __VERIFIER_nondet_float();
	__VERIFIER_assume(g <= 0x1.0p-149f);
	__VERIFIER_assume(g > 0.0f);

	// exponent is >= 107 - -127 = 234 = 255 - 21
	float div = f / g;

	assert(!(div == 0.0));

	return 1;
}
// Imported from ESBMC regression/floats/Float-div2
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
