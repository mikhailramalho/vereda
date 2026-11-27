int main() {
	int a;
	__VERIFIER_assume(a % 2 == 0);
	int obj1;
	char obj2;
	int *ptr = a % 2 == 0 ? &obj1 : &obj2; 
        memset(ptr, 0, 4); // We are definitely at &obj1
	return 0;
	
}
// Imported from ESBMC regression/cstd/intrinsic_memset_guard_true
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
