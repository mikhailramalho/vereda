#include <assert.h>
#include <stdbool.h>

bool t2()
{
	bool x;
	return x + 1;
}

bool t1()
{
	return 1 == 2 && t2();
}

void test(bool x)
{
}

int main()
{
	int nS;
	int nE = t1();
	(nS != nE) && t1() || (2 == 3);
}
// Imported from ESBMC regression/goto-coverage/github_1999_7
// Needs: --condition-coverage-claims
// RUN: --condition-coverage-claims --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^Short Circuited Conditions:  2
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
