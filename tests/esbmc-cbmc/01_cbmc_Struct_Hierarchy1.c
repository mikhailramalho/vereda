struct tag1 {
	struct tag2 {
		int f;
	} y;
} x;

int main() {
	x.y.f = 0;
}
// Imported from ESBMC regression/cbmc/01_cbmc_Struct_Hierarchy1
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
