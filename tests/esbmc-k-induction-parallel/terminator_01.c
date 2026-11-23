int __VERIFIER_nondet_int();

int main()
{
  int x=__VERIFIER_nondet_int();
  int *p = &x;
 
  while(x<100) {
   (*p)++;
  }                       
    
  return 0;
}

// Imported from ESBMC regression/k-induction-parallel/terminator_01
// Needs: --k-induction-parallel
// RUN: --k-induction-parallel
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
