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
	bool x = ((nS != nE) && t1() || (2 == 3));
}
// Imported from ESBMC regression/goto-coverage/github_1999_6
// Needs: --condition-coverage-claims --verbosity coverage:10
// RUN: --condition-coverage-claims --verbosity coverage:10
// MODE: FUTURE
// EXPECT: ^Short Circuited Conditions:  2
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
