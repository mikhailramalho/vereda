extern int __VERIFIER_nondet_int(void);
int value;
int main(void)
{
  value = __VERIFIER_nondet_int();
  __VERIFIER_assert(value != 1); // target
}
// Imported from ESBMC regression/witnesses_validate/reachsafety-violationwitness/nondet_assignment_return_c_expression_avoid
// Needs: --validate-violation-witness --witness witness.yml
// RUN: -m32 --ub-pointer-check=false --validate-violation-witness --witness witness.yml
// EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none
