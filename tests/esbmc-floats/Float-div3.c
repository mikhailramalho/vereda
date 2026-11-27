#include <assert.h>
#include <math.h>

float __VERIFIER_nondet_float (void);

int main (void) {
	float f = __VERIFIER_nondet_float();
	__VERIFIER_assume(f < +INFINITY);
	__VERIFIER_assume(f > 1.0f);

	float g = __VERIFIER_nondet_float();
	__VERIFIER_assume(g < 1.0f);
	__VERIFIER_assume(g >= 0x1.0p-126f);
	__VERIFIER_assume(g > 0.0f);

	float div = f / g;

	assert(!(div == 0.0));

	return 1;
}
// Imported from ESBMC regression/floats/Float-div3
// RUN: --no-incremental-bmc
// MODE: THOROUGH
// EXPECT: ^VERIFICATION SUCCESSFUL$
