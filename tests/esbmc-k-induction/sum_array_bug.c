int __VERIFIER_nondet_int();

main()
{
  unsigned int M;
  int A[M], B[M], C[M];
  unsigned int  i;
  
  for(i=0;i<M;i++)
    A[i] = __VERIFIER_nondet_int();
  
  for(i=0;i<M;i++)
    B[i] = __VERIFIER_nondet_int();

  for(i=0;i<M;i++)
     C[i]=A[i]+B[i];
  
  for(i=0;i<M;i++)
     assert(C[i]==A[i]-B[i]);
}

// Imported from ESBMC regression/k-induction/sum_array_bug
// Needs: --k-induction
// RUN: --k-induction
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
