#include <string.h>

const int b;

int main(int argc, char **argv)
{
	int *c = argc > 1 ? &b : &(int){4};
	*c = 42;
}

// Imported from ESBMC regression/esbmc/github_1548-cond-lit
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: \bdereference failure: write access to const object `b'
