int main() {
	int i, j=__VERIFIER_nondet_int();

	i = 1;
	if(j > 0)
	  j += i ;
//		j += i ;
//	else
//		j = 0;

	assert(i != j);
}
// Imported from ESBMC regression/cbmc/01_cbmc_if1
// EXPECT: ^VERIFICATION SUCCESSFUL$
