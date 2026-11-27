#define a 2
int main()
{
  unsigned long long int i=1, sn=0;
  while ( 1 ) {
    sn = sn + ((i%10==9)? 4 : a);
    assert ( sn==i*a ) ;
    i++;
  }
}
// Imported from ESBMC regression/k-induction/sum06_bug02
// Needs: --k-induction
// RUN: --k-induction
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
