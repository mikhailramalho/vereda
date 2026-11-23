int main(void)
{
  ERROR: __VERIFIER_assert(0); // error label
}
// Imported from ESBMC regression/witnesses_validate/reachsafety-violationwitness/target_error_label_column
// Needs: --error-label ERROR --validate-violation-witness --witness witness.yml
// RUN: --error-label ERROR --validate-violation-witness --witness witness.yml
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
