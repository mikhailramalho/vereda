#include <assert.h>

int arr[16];

void f(unsigned k, unsigned v)
{
	k %= 8;
	arr[k] = v;
	void *p = arr;
	int *q = (int *)(void *)((short *)p + (k * 2));
	int w = *q;
	assert(w == v);
}
// Imported from ESBMC regression/esbmc/github_1220-success
// Needs: --func f
// RUN: --func f
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
