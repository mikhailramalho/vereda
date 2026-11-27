extern void __VERIFIER_error(void);
extern unsigned short __VERIFIER_nondet_ushort(void);
int main(){
	unsigned short T2_460 = __VERIFIER_nondet_ushort();
	if((unsigned int) ((unsigned int) (((unsigned short)(T2_460)) + ((unsigned int) 0))) <= (unsigned int) ((unsigned int) 65536)){
		if(!((unsigned int) (((unsigned int) 112) - ((unsigned int) (((unsigned short)(T2_460)) + ((unsigned int) 72)))) < (unsigned int) ((unsigned int) 63))){
			__VERIFIER_error();
		}
	}    
       return 0;
}
// Imported from ESBMC regression/esbmc/github_1363_invalid_interval_expression_fail
// Needs: --interval-analysis
// RUN: --interval-analysis --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
