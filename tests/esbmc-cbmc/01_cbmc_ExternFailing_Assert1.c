int main() {
	int i, j;

	i = 1;
	j = 2;

	assert(i == j);
}
// Imported from ESBMC regression/cbmc/01_cbmc_ExternFailing_Assert1
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
