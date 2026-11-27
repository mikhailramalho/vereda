int x = 0;

void f1(void) {
	x = 1;
}

void call(void (*f)()) {
	f();
}

int main() {
	call(f1);
}
// Imported from ESBMC regression/cbmc/01_cbmc_Function_Pointer1
// EXPECT: ^VERIFICATION SUCCESSFUL$
