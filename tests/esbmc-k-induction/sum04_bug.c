#define a (2)
#define SIZE 8
unsigned int __VERIFIER_nondet_uint();
int main() { 
  int i, sn=0;
  for(i=1; i<=SIZE; i++) {
    if (i<4) //first case
    sn = sn + a; //second case OK
    //if (i==4) sn=-10; //third case OK
  }
  //__VERIFIER_assume(i>n);
  assert(sn==SIZE*a || sn == 0);
}
// Imported from ESBMC regression/k-induction/sum04_bug
// Needs: --k-induction
// RUN: --k-induction
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
