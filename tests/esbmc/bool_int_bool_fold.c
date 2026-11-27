// Widening a _Bool yields exactly 0 or 1, so comparing it against zero
// recovers the bool. The simplifier folds that round trip away; these
// assertions pin the semantics it must preserve while doing so. #4626.
_Bool __VERIFIER_nondet_bool(void);
int __VERIFIER_nondet_int(void);

int main(void)
{
  _Bool b = __VERIFIER_nondet_bool();

  __VERIFIER_assert(((int)b != 0) == b); // (int)b != 0 is b
  __VERIFIER_assert(((int)b == 0) == !b); // (int)b == 0 is !b
  __VERIFIER_assert((_Bool)(int)b == b); // (_Bool)(int)b is b

  if ((int)b != 0)
    __VERIFIER_assert(b); // b holds on the taken branch
  else
    __VERIFIER_assert(!b); // b fails on the other branch

  // The reverse trip keeps only whether i was non-zero, so it is not the
  // identity on i. Folding it away would be unsound.
  int i = __VERIFIER_nondet_int();
  __VERIFIER_assume(i == 2);
  __VERIFIER_assert((int)(_Bool)i == 1); // (int)(_Bool)2 is 1, not 2

  return 0;
}
// Imported from ESBMC regression/esbmc/bool_int_bool_fold
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
