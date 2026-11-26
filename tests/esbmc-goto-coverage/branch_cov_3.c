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

// Imported from ESBMC regression/goto-coverage/branch_cov_3
// Needs: --branch-function-coverage --k-induction
// RUN: --branch-function-coverage --k-induction
// EXPECT: ^Function Entry Points & Branches : 8
// EXPECT: ^Reached : 5
// EXPECT: ^Branch Coverage: 62.5%
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// MODE: FUTURE
// Vereda: verdict none
