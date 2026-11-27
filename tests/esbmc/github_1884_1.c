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
// Imported from ESBMC regression/esbmc/github_1884_1
// RUN: --ub-signed-overflow-check --unsigned-overflow-check --ub-shift-check
// EXPECT: ^VERIFICATION FAILED$
