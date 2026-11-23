extern int __VERIFIER_nondet_int(void);
int value;
int main(void)
{
  value = __VERIFIER_nondet_int();
  __VERIFIER_assert(value != 1); // target
}
// Imported from ESBMC regression/witnesses_validate/reachsafety-violationwitness/nondet_assignment_return_c_expression_follow
// Needs: --validate-violation-witness --witness witness.yml
// RUN: --32 --no-pointer-check --no-bounds-check --validate-violation-witness --witness witness.yml
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
