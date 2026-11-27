int __VERIFIER_nondet_int();
_Bool __VERIFIER_nondet_bool();

main()
{
  int x, y, z;

  while (x>0 && y>0 && z>0)
  {
    _Bool c1 = __VERIFIER_nondet_bool();
    _Bool c2 = __VERIFIER_nondet_bool();
    if(c1) {
      x=x-1;
    } else if (c2) {
      y = y- 1;
      z=__VERIFIER_nondet_int();
    } else {
      z=z-1;
      x=__VERIFIER_nondet_int();
    }
  }                           
  assert(0);    
}


// Imported from ESBMC regression/k-induction-parallel/terminator_06_bug
// Needs: --k-induction-parallel
// RUN: --k-induction-parallel
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
