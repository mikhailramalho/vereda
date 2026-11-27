int main()
{
	int tt = __VERIFIER_nondet_int();
	int res = tt + 1;
	int a = res > 1 ? res / 2 : res;
	__VERIFIER_assert(a == 2); // Failed as expected
}
// Imported from ESBMC regression/esbmc/github_447-1
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
