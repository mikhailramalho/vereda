#include <math.h>
#include <assert.h>

int main()
{
	assert(!isfinite(NAN));
}
// Imported from ESBMC regression/floats-regression/nan-finite
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
