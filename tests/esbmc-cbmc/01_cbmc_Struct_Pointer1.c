struct tag1 {
	int f;
};

int main() {
	struct tag1 x;
	struct tag1 *px;

	px = &x;
	px->f = 0;
	(*px).f = 0;
}
// Imported from ESBMC regression/cbmc/01_cbmc_Struct_Pointer1
// EXPECT: ^VERIFICATION SUCCESSFUL$
