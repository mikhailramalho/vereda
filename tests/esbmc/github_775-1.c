#include <stdlib.h>
#include <assert.h>

void steal_addr_space(size_t n)
{
	char *a = malloc(n);
	free(a);
}

int main()
{
	size_t MiB = 1ULL << 20;
	/* 16 * 256 MiB = 4 GiB */
	for (int i=0; i<16; i++)
		steal_addr_space(256 * MiB);
	assert(0);
}
// Imported from ESBMC regression/esbmc/github_775-1
// Needs: REQUIRES arch32
// RUN: --32 --force-malloc-success
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
