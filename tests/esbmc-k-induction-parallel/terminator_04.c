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
  assert(!(x>0 && y>0));    
}


// Imported from ESBMC regression/k-induction-parallel/terminator_04
// Needs: --k-induction-parallel
// RUN: --k-induction-parallel --std=gnu89
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
