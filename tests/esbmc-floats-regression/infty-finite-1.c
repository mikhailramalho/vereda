#include <math.h>
#include <assert.h>

int main()
{
	assert(!isfinite(INFINITY));
}
// Imported from ESBMC regression/floats-regression/infty-finite-1
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
