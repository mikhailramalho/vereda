#define a (2)
int __VERIFIER_nondet_int();
unsigned int __VERIFIER_nondet_uint();
_Bool __VERIFIER_nondet_bool();

int main() { 
  int sn=0;
  unsigned int x=0;

  while(1){
    sn = sn + a;
    x++;
    assert(sn==x*a || sn == 0);
  }
}
// Imported from ESBMC regression/k-induction/sum03
// Needs: --k-induction
// RUN: --k-induction
// MODE: FUTURE
// EXPECT: ^VERIFICATION UNKNOWN$
// Vereda: verdict none
