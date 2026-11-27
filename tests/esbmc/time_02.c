#include <time.h>

int main()
{
	int t;

	time(&t);
	return 0;
}

// Imported from ESBMC regression/esbmc/time_02
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
