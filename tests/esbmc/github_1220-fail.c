#include <assert.h>

int arr[16];

void f(unsigned k, unsigned v)
{
	k %= 16;
	arr[k] = v;
	void *p = arr;
	int *q = (int *)(void *)((short *)p + (k * 1));
	int w = *q;
	assert(w == v);
}
// Imported from ESBMC regression/esbmc/github_1220-fail
// RUN: --entry f --multi-property --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: \bdereference failure: Incorrect alignment when accessing data object\b
// Vereda: verdict FAILED
