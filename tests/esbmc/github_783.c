#include <stdlib.h>

void foo(void *p)
{
	return free(p);
}

int main()
{
	foo(malloc(5));
}
// Imported from ESBMC regression/esbmc/github_783
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
