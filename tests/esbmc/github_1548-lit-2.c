#include <string.h>

int main(int argc, char **argv)
{
	static const char *b[3];

	char *c = (void *)b;
	*c = 42;
}

// Imported from ESBMC regression/esbmc/github_1548-lit-2
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
