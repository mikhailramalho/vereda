#include <string.h>
#include <assert.h>

int main()
{
	int no = __VERIFIER_nondet_int();
	const char *err = strerror(no);
	assert(err); // C99 says this must not be NULL
}
// Imported from ESBMC regression/esbmc/strerror-no-overflow
// RUN: --overflow-check
// MODE: THOROUGH
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
