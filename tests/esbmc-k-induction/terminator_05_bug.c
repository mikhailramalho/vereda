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
  assert(0);    
}


// Imported from ESBMC regression/k-induction/terminator_05_bug
// Needs: --k-induction
// RUN: --k-induction
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
