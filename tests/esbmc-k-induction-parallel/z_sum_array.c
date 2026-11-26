
main()
{
  unsigned int M;
  int A[M], B[M], C[M];
  
  for(unsigned int i=0; i<M; i++)
    A[i] = __VERIFIER_nondet_int();
  
  for(unsigned int j=0;j<M;j++)
    B[j] = __VERIFIER_nondet_int();

  for(unsigned int k=0;k<M;k++)
     C[k]=A[k]+B[k];
  
  for(unsigned int l=0;l<M;l++)
     assert(C[l]==A[l]+B[l]);
}

// Imported from ESBMC regression/k-induction-parallel/z_sum_array
// Needs: --k-induction-parallel
// RUN: --k-induction-parallel -m32
// EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: KNOWNBUG
// Vereda: verdict none
