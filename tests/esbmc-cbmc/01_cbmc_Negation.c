int test = 0;

int main() {
	test = ~test;
}
// Imported from ESBMC regression/cbmc/01_cbmc_Negation
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
