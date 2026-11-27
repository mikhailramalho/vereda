int main()
{
  int value;
  __VERIFIER_assert(__ESBMC_exists(&value, value == 42)); // exists 42
  __VERIFIER_assert(
    !__ESBMC_exists(&value, value == 0 && value == 1)); // contradiction
  return 0;
}
// Imported from ESBMC regression/bitwuzla/quantifiers-exists
// Needs: __ESBMC_exists
// RUN: --solver bitwuzla --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
