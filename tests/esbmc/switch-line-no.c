#include <assert.h>

int main()
{
	int data = 0;
	switch (42) {
	case 42: { // line 7
		data = __VERIFIER_nondet_int(); // line 8
		break;
	}
	default:
		break;
	}
	assert(!data);
}
// Imported from ESBMC regression/esbmc/switch-line-no
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: switch\-line\-no\.c line 8.*\n.*\n\s*data =
// Vereda: unmet EXPECT: switch\-line\-no\.c line 8.*\n.*\n\s*data =
// MODE: FUTURE
// Vereda: verdict FAILED
