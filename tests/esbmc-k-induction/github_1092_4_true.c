int status = 0;
int main()
{
  // status = *
  while(__VERIFIER_nondet_int()) // __verifier_nondet_int$1 = *
  {
    if(!status) // status = *
    {
      status = 1;
    }
    else if(status == 1)
    {
      status = 2;
    }
  } // status = 1 U 2
  do
  {
    __VERIFIER_assert(status != 3);
  } while(0);
  return 0;
}
// Imported from ESBMC regression/k-induction/github_1092_4_true
// Needs: --k-induction
// RUN: --k-induction
// MODE: KNOWNBUG
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
