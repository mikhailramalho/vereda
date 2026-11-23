extern int __VERIFIER_nondet_int(void);

int main() 
{
  int n = __VERIFIER_nondet_int();
  while (n < 10 ) n++;
  return 0;
}
// Imported from ESBMC regression/termination/termination11
// Needs: --k-induction --termination
// RUN: --k-induction --termination
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
