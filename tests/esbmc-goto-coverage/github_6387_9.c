void helper(int x)
{
  __VERIFIER_assert(x != 7); // helper never sees 7
}
void target(int y)
{
  __VERIFIER_assert(y == y); // target trivial
  helper(y);
}
int main()
{
  int z;
  target(z);
  return 0;
}
// Imported from ESBMC regression/goto-coverage/github_6387_9
// Needs: --assertion-coverage --witness-output-yaml w.yaml SEED_FILE base-k0-0-w.yaml SENTINEL_NOT_OVERWRITTEN CHECK_FILE base-k0-0-w.yaml contains SENTINEL_NOT_OVERWRITTEN
// RUN: --multi-property --assertion-coverage --function target --witness-output-yaml w.yaml --no-incremental-bmc
// MODE: FUTURE
// EXPECT: claim\(s\) outside the coverage instrumentation were violated
// Vereda: verdict none
