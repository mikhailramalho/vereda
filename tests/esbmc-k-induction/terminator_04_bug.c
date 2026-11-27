int __VERIFIER_nondet_int();
_Bool __VERIFIER_nondet_bool();

main()
{
  int x=__VERIFIER_nondet_int();
  int y=__VERIFIER_nondet_int();

  while (x>0 && y>0)
  {
    _Bool c = __VERIFIER_nondet_bool();
    if(c) 
      x=x-1;
    else {
      x = __VERIFIER_nondet_int();
      y = y- 1;
    }
  }                           
  assert(0);    
}


// Imported from ESBMC regression/k-induction/terminator_04_bug
// Needs: --k-induction
// RUN: --k-induction
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
