int main(void)
{
  int *p = 0;
  return *p;
}
// Imported from ESBMC regression/witnesses_validate/memsafety-violationwitness/target_full_expression_column
// Needs: --sv-comp --validate-violation-witness --witness witness.yml
// RUN: --sv-comp --no-assertions --validate-violation-witness --witness witness.yml
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
