#include <assert.h>
int main()
{
	int a, x, y = 0, z;
	a = __VERIFIER_nondet_int();
	x = __VERIFIER_nondet_int();
	z = __VERIFIER_nondet_int();
	if (a > 25000 && x == 30000)
	{
		while (y++ < z)
		{
			if (y % 3 != 0)
			{
				a++;
				x--;
			}
			else
			{
				a--;
				x++;
			}
			assert(a != x);
		}
	}
}
// Imported from ESBMC regression/goto-coverage/github_2063_1
// Needs: --assertion-coverage
// RUN: --unwind 4 --assertion-coverage
// MODE: FUTURE
// EXPECT: ^Total Asserts: 1
// EXPECT: ^Total Assertion Instances: unknown / non-deterministic
// EXPECT: ^Reached Assertion Instances: 4
// EXPECT: ^Assertion Instances Coverage Unknown
// EXPECT: ^COVERAGE ANALYSIS INCOMPLETE
// Vereda: verdict none
