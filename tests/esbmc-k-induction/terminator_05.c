int __VERIFIER_nondet_int();
_Bool __VERIFIER_nondet_bool();

main()
{
  int x, y, d;

  while (x>0 && y>0 && d>0)
  {
    _Bool c = __VERIFIER_nondet_bool();
    if(c) {
      x=x-1;
      d=__VERIFIER_nondet_int();
    } else {
      x = __VERIFIER_nondet_int();
      y = y- 1;
      d=d-1;
    }
  }                           
  assert(!(x>0 && y>0 && d>0));    
}


// Imported from ESBMC regression/k-induction/terminator_05
// Needs: --k-induction --std=gnu89
// RUN: --k-induction --std=gnu89
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
