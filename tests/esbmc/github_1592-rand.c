#include <stdlib.h>

int main()
{
	int v = rand();
	assert(v != RAND_MAX);
}
// Imported from ESBMC regression/esbmc/github_1592-rand
// EXPECT: ^VERIFICATION FAILED$
