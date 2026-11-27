extern void reach_error(void);
int main(void)
{
  __VERIFIER_assert(0); // dereference failure: unrelated application condition
  reach_error();
}
// Imported from ESBMC regression/witnesses_validate/memsafety-violationwitness/target_after_user_assertion_message
// Needs: --validate-violation-witness --witness witness.yml
// RUN: -m32 --std c11 --validate-violation-witness --witness witness.yml
// EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict none
