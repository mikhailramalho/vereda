#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

int main()
{
	unsigned n = __VERIFIER_nondet_uint() % 1024;
	char *fmt = malloc(n+1);
	uint16_t *data = malloc((n+1) * sizeof(*data));
	if (fmt && data)
	{
		unsigned i = __VERIFIER_nondet_uint() % n;
		fmt[n] = '\0';
		printf(fmt, data[i]);
	}
}
// Imported from ESBMC regression/esbmc/github_1172-1
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: \bdivision by zero\b
