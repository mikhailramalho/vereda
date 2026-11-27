#include <string.h>

const int b;

int main(int argc, char **argv)
{
	char *c = (void *)"bli bla blo";
	*c = 42;
}

// Imported from ESBMC regression/esbmc/github_1548-lit
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: \bdereference failure: write access to string constant
