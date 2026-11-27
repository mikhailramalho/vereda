
#include <assert.h>
#include <stdlib.h>
#include <string.h>

struct incomplete;

extern struct incomplete JJ;

int main()
{
	int k = 42;
	memcpy(&JJ, &k, sizeof(k));
	int j;
	memcpy(&j, &JJ, sizeof(k));
	assert(j == 42);
}

// struct incomplete { short x, y; };
// Imported from ESBMC regression/esbmc/github_1210-2-struct
// FILES: incomplete-lib.c
// EXPECT: ^VERIFICATION SUCCESSFUL$
