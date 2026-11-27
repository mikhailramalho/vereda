#define a 2
int main()
{
  unsigned long long int i=1, sn=0;
  unsigned int n=20;
  while ( i<=n ) {
    sn = sn + ((i%15==14)? 4 : a);
    i++;
  }
  assert ( sn==n*a ) ;
}
// Imported from ESBMC regression/k-induction/sum06_bug04
// Needs: --k-induction
// RUN: --k-induction
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
