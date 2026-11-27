int main() {
	int k = 5 / 2;

	assert(k == 2);
}
// Imported from ESBMC regression/cbmc/01_cbmc_Division
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
