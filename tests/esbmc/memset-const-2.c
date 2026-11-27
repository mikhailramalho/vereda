#include <string.h>

const char b[] = "abc";

int main(int argc, char **argv)
{
	char a[2];
	char *c = argc == 1 ? a : b;
	memset(c, 0, 1);
}
// Imported from ESBMC regression/esbmc/memset-const-2
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
