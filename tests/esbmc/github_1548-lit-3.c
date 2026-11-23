#include <string.h>

int main(int argc, char **argv)
{
	static const char *const b[3];

	char *c = (void *)b;
	*c = 42;
}

// Imported from ESBMC regression/esbmc/github_1548-lit-3
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: \bdereference failure: write access to const object `b'
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
