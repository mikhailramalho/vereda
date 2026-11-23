int status = 0;
int foo()
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
}

int bar()
{
  do
  {
    __VERIFIER_assert(status != 2);
  } while(0);
}
int main()
{
  foo();
  bar();
  return 0;
}
// Imported from ESBMC regression/k-induction/github_1092_5_false
// Needs: --k-induction
// RUN: --k-induction
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
