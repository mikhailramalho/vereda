
#include <assert.h>
#include <math.h>

int main()
{
	double x = INFINITY;
	assert(x == x);
}
// Imported from ESBMC regression/esbmc/github_1226-3
// EXPECT: ^VERIFICATION SUCCESSFUL$
