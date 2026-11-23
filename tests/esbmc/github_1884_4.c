extern void __VERIFIER_error(void);
unsigned int id(int x){
    return x;
}

int main(){
    if((4294967295U+ 1U) - id(4294967295U) < 0U){
	__VERIFIER_error();
    }
    return 0;
}
// Imported from ESBMC regression/esbmc/github_1884_4
// Needs: --interval-analysis
// RUN: --interval-analysis
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
