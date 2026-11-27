extern void __VERIFIER_error(void);

extern long __VERIFIER_nondet_long(void);
extern _Bool __VERIFIER_nondet_bool(void);

int main(){
        _Bool b1 = __VERIFIER_nondet_bool();
        _Bool b2 = __VERIFIER_nondet_bool();
        long l1 = __VERIFIER_nondet_long();
        long l2 = __VERIFIER_nondet_long();
        if(b2){
                if(!(((b1) || ((l2) == (0))) && (!(b2)))){
                        if(!(((b2) || ((l1) == (0))) && (!(b1)))){
                        __VERIFIER_error();
                        }
                }
        }
    return 0;
}
// Imported from ESBMC regression/esbmc/github_1565-1
// Needs: --interval-analysis --interval-analysis-wrapped
// RUN: --interval-analysis --interval-analysis-wrapped
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
