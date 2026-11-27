#include <assert.h>

/* The big-endian layout from #571. */

struct S {
	unsigned a : 4;
	unsigned b : 4;
	unsigned c : 4;
	unsigned d : 4;
};

int main()
{
	union {
		struct S s;
		unsigned short sh;
	} u = { { .a = 1, .b = 2, .c = 3, .d = 4 } };
	assert(u.sh == 0x1234);
}
// Imported from ESBMC regression/esbmc/github_571_1
// Needs: --big-endian
// RUN: --big-endian --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
