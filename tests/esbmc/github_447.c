#define UINT8  unsigned __int8
unsigned arr[100];
int tt;

int foo() {
	int x;
    tt = __VERIFIER_nondet_int();
	tt += 1;
	int y = 25;
	int res = tt + y;

	return res;
}

int loop_init() {
	//__ESBMC_init_object(&arr);
	int a = foo();
	while (a > 1)
		a = a / 2;
	__VERIFIER_assert(a == 2); // Failed as expected

	return 0;
}
// Imported from ESBMC regression/esbmc/github_447
// Needs: --k-induction
// RUN: --function loop_init --k-induction
// MODE: FUTURE
// EXPECT: \n\s*res = ([^ ]+) .*\n.*\n.*\n.*\s*a = \1
// Vereda: verdict none
