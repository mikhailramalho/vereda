extern void reach_error(void);
int main(void)
{
  int a[1] = {0};
  int i = 1;
  __VERIFIER_assert(a[i] == 0); // application condition
  reach_error();
}
// Imported from ESBMC regression/witnesses_validate/memsafety-violationwitness/target_after_invalid_array_assert_guard
// Needs: --validate-violation-witness --witness witness.yml
// RUN: -m32 --std c11 --validate-violation-witness --witness witness.yml --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none
