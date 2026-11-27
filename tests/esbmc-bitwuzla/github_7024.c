int main()
{
  int idx_vfy;

  idx_vfy = 17;

  __VERIFIER_assert(
    __ESBMC_forall(&idx_vfy, !(0 <= idx_vfy && idx_vfy < 10) || idx_vfy == 13)
); // forall eq
}
// Imported from ESBMC regression/bitwuzla/github_7024
// Needs: __ESBMC_forall
// RUN: --solver bitwuzla --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^\[COUNTEREXAMPLE\]$
