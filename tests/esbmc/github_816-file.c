#include <assert.h>

int x = (int){42};

int main()
{
	assert(x == 42);
}
// Imported from ESBMC regression/esbmc/github_816-file
// EXPECT: ^VERIFICATION SUCCESSFUL$
