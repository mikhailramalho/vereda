#include <string.h>

const int b;

int main(int argc, char **argv)
{
	int *c = &b;
	*c = 42;
}

// Imported from ESBMC regression/esbmc/github_1548
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: \bdereference failure: write access to const object `b'
