
#include <assert.h>
#include <stdint.h>

int arr[16];

void f(unsigned k, unsigned v)
{
	k %= 16;
	arr[k] = v;
	void *p = arr;
	int w = *(int *)(void *)((short *)arr + (k * 1));
	assert(w == v);
}

int main(int argc, char **argv)
{
	f(atoi(argv[1]), atoi(argv[2]));
}
// Imported from ESBMC regression/esbmc/align-deref_fail
// Needs: --func f
// RUN: --func f -Wno-error=implicit-function-declaration
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: \bdereference failure: Incorrect alignment when accessing data object\b
// Vereda: verdict none
