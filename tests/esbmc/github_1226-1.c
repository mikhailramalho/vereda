int main()
{
    double x = __VERIFIER_nondet_float();

	__VERIFIER_assume(x >= 0);
	
	__VERIFIER_assert(x == x);

    return 0;
}
// Imported from ESBMC regression/esbmc/github_1226-1
// EXPECT: ^VERIFICATION SUCCESSFUL$
