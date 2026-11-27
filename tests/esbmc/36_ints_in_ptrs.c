#include <stdbool.h>

int
main()
{
	void *face, *bees;
	int num;

	bees = &num;
	face = (void*)1234;
	num = (int)face;

	assert(num == 1234);

	face = (__VERIFIER_nondet_bool()) ? (void*)1234 : (void*)1234;
	num = (int)face;

        assert(num == 1234);

	return 0;
}
// Imported from ESBMC regression/esbmc/36_ints_in_ptrs
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
