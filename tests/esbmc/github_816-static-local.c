#include <assert.h>

int f()
{
	static int x = (int){42};
	return x++;
}

int main()
{
	int z1 = f();
	int z2 = f();
	assert(z1 == 42);
	assert(z2 == 43);
}
// Imported from ESBMC regression/esbmc/github_816-static-local
// EXPECT: ^VERIFICATION SUCCESSFUL$
