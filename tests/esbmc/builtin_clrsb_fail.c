// clrsb(-2) is 30: of the 31 bits below the sign bit, only the first is a
// redundant copy of it. Exactly the wrong expectation of 31 must fail.
int __VERIFIER_nondet_int(void);

int main(void)
{
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assume(x == -2);
  __VERIFIER_assert(__builtin_clrsb(x) == 30); // clrsb(-2) is 30
  __VERIFIER_assert(__builtin_clrsb(x) == 31); // clrsb(-2) == 31 is wrong
  return 0;
}
// Imported from ESBMC regression/esbmc/builtin_clrsb_fail
// RUN: --ub-count-zero-check --multi-property
// EXPECT: ^VERIFICATION FAILED$
