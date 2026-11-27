#include <assert.h>

struct str {
	union { int v; };
};

struct str arr[2] = { { 42 }, { 23 } };

int main()
{
	struct str *p = arr+1;
	int x = (p--)->v;
	assert(x == 23);
}
// Imported from ESBMC regression/esbmc/github_613_postdecrement_success
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
