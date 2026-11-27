int linear_search(int *a, int n, int q) {
  unsigned int j=0;
  while (j<n && a[j]!=q) {
  j++;
  if (j==20) j=-1;
  }
  if (j<n) return 1;
  else return 0;
}
int main() { 
  unsigned int  SIZE=__VERIFIER_nondet_uint()/2+1;
  int a[SIZE];
  a[SIZE/2]=3;
  assert(linear_search(a,SIZE,3));
}
// Imported from ESBMC regression/k-induction/linear_search_bug
// Needs: --k-induction
// RUN: --k-induction
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
