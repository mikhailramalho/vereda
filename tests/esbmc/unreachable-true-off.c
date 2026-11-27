
int main()
{
	if(__VERIFIER_nondet_bool())
		__ESBMC_unreachable();
}
// Imported from ESBMC regression/esbmc/unreachable-true-off
// Needs: __ESBMC_unreachable
// RUN: --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
