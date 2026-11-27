unsigned int __VERIFIER_nondet_uint();
int __VERIFIER_nondet_int();

#define SIZE 1

main()
{
  unsigned int j,k;
  int array[SIZE], menor;
  
  menor = __VERIFIER_nondet_int();

  for(j=0;j<SIZE;j++) {
       array[j] = __VERIFIER_nondet_int();
       
       if(array[j]<=menor)
          menor = array[j];                          
    }                       
    
    assert(array[0]>menor);    
}

// Imported from ESBMC regression/k-induction/array_bug
// Needs: --k-induction
// RUN: --k-induction
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
