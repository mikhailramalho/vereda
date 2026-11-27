#include <string.h>

const int b = 6;

int main(int argc, char **argv)
{
	memset(&b, 0, 1);
}
// Imported from ESBMC regression/esbmc/github_1548-2
// EXPECT: ^VERIFICATION FAILED$
