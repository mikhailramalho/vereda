#define a 2
int main()
{
  unsigned long long int i=1, sn=0;
  while ( 1 ) {
    sn = sn + ((i%10==9)? 4 : a);
    i++;
    assert ( sn==(i-1)*a ) ;
  }
}
// Imported from ESBMC regression/k-induction/sum06_bug03
// Needs: --k-induction
// RUN: --k-induction
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
