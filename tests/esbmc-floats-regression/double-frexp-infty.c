#include <math.h>
#include <assert.h>

int main()
{
	int e;
	double r = frexp(INFINITY, &e);
	assert(isinf(r));
}
// Imported from ESBMC regression/floats-regression/double-frexp-infty
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict FAILED
