int __VERIFIER_nondet_int();
_Bool __VERIFIER_nondet_bool();

main()
{
  int x=__VERIFIER_nondet_int();
  int y=__VERIFIER_nondet_int();

  if (y>0)
  {
    while(x<100) 
    {
      x=x+y;
     }
  }                           
  assert(0);    
}


// Imported from ESBMC regression/k-induction/terminator_03_bug
// Needs: --k-induction
// RUN: --k-induction
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
