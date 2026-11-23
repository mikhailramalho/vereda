#include <assert.h>

void foo(unsigned int x)
{
	x++;
	assert(x > 3);
}

int main()
{
	foo(3435973937); // hold
}
// Imported from ESBMC regression/esbmc/github_2083_1
// Needs: --assign-param-nondet
// RUN: --function foo --assign-param-nondet
// MODE: FUTURE
// EXPECT: \bx = 0\b
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
