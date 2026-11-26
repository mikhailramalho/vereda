extern int __VERIFIER_nondet_int(void);
extern void reach_error(void);
int main(void)
{
  int state = __VERIFIER_nondet_int();
  while (state < 1)
  {
    if (state == 0)
    {
      int a = __VERIFIER_nondet_int();
      if (a != 1)
        return 0;
      state = 1;
    }
    else
    {
      int b = __VERIFIER_nondet_int();
      if (b != 0)
        return 0;
      state = 2;
    }
  }
  reach_error();
}
// Imported from ESBMC regression/witnesses_validate/reachsafety-violationwitness/return_mutually_exclusive_branches
// Needs: --sv-comp --validate-violation-witness --witness witness.yml
// RUN: -m32 --sv-comp --std c11 --validate-violation-witness --witness witness.yml --unwind 4
// EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none
