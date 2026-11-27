#define a 2
int __VERIFIER_nondet_int();

int main()
{
  unsigned long long int i=1, sn=0;
  int n = __VERIFIER_nondet_int();

  __VERIFIER_assume(n < 1000 && n >= -1000);

  while ( i <= n ) {
    sn = sn + ((i%10==9)? 4 : a);
    i++;
  }

  assert(sn == n * a || sn == 0);
}
// Imported from ESBMC regression/k-induction/sum06_bug
// Needs: --k-induction
// RUN: --k-induction
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
