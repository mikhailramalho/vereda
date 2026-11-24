
int main()
{
	if(0)
		__ESBMC_unreachable();
}
// Imported from ESBMC regression/esbmc/unreachable-false-off
// Needs: __ESBMC_unreachable
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
