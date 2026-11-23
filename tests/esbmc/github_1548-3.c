#include <string.h>

const char b[] = "abc";

int main(int argc, char **argv)
{
	char a[2];
	char *c = argc == 1 ? a : b;
	memset(c, 0, 1);
}
// Imported from ESBMC regression/esbmc/github_1548-3
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: \bdereference failure: write access to const object `b'
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
