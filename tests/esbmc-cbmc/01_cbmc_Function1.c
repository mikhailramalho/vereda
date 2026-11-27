int f(int a) {
	return a + 1;
}

// late prototype
int f(int);

int a[1];

int main() {
	int x, y;

	a[0] = y;
	a[0] = f(a[0]);

	assert(a[0] == y+1);
}
// Imported from ESBMC regression/cbmc/01_cbmc_Function1
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
