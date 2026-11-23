int __VERIFIER_nondet_int();
int g;

void cs(void)
{
  if (__VERIFIER_nondet_int())
    g++;
}

void t2(void)
{
  cs();
}

void t1(void)
{
  cs();
  if (__VERIFIER_nondet_int())
    g += 10;
}

int main()
{
  void (*f)(void) = __VERIFIER_nondet_int() ? t1 : t2;
  f();
  __VERIFIER_assert(g != 11); // g
  return 0;
}
// Imported from ESBMC regression/witnesses_validate/reachsafety-violationwitness/branching_in_other_fp_target
// Needs: --validate-violation-witness --witness witness.yml
// RUN: --validate-violation-witness --witness witness.yml
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
