// The inner loop sees j = 0 at its head on every entry. That is no cycle, as
// the state saved on one entry must not be compared on the next.
int main(void)
{
  int a[3] = {1, 1, 0};
  for (int x = 0; x < 3; x++)
  {
    int j = 0;
    while (a[j] != 0)
      j++;
  }
  return 0;
}
// Imported from ESBMC regression/termination/recurrent_state_reentry
// Needs: --state-hashing --add-symex-value-sets --no-assertions --termination --max-inductive-step 3 --interval-analysis
// RUN: --no-div-by-zero-check --state-hashing --add-symex-value-sets --no-align-check --k-step 2 --no-vla-size-check --no-pointer-check --no-bounds-check --no-assertions --termination --max-inductive-step 3 --interval-analysis
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
