#include <stdlib.h>
#include <assert.h>

int main()
{
	size_t GiB = 1ULL << 30;
	if (malloc(2 * GiB))
		assert(!malloc(2 * GiB));
}
// Imported from ESBMC regression/esbmc/github_785-2
// Needs: REQUIRES arch32
// RUN: --32
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
