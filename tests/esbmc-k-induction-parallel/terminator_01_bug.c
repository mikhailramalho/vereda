int __VERIFIER_nondet_int();

main()
{
  int x=__VERIFIER_nondet_int();
  int *p = &x;
 
  while(x<100) {
   (*p)++;
  }                       
  assert(0);    
  //  assert(array[0]>=menor);    
}

// Imported from ESBMC regression/k-induction-parallel/terminator_01_bug
// Needs: --k-induction-parallel
// RUN: --k-induction-parallel
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
