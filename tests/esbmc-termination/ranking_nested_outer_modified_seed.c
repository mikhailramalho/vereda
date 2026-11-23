/* x = 0 holds on the first entry to the inner loop only: the outer
 * loop increments x before re-entering it. With x == 1 the inner loop
 * resets j forever, so the ranking check must not seed x == 0. */
int main(void)
{
  for (int x = 0; x < 2; x++)
  {
    int j = 0;
    while (j < 2)
    {
      j++;
      if (x == 1)
        j = 0;
    }
  }
  return 0;
}
// Imported from ESBMC regression/termination/ranking_nested_outer_modified_seed
// Needs: --force-realloc-success --state-hashing --add-symex-value-sets --k-step 2 --no-assertions --termination --max-inductive-step 3 REQUIRES arch32
// RUN: --no-div-by-zero-check --force-malloc-success --force-realloc-success --state-hashing --add-symex-value-sets --no-align-check --k-step 2 --floatbv --no-vla-size-check --32 --no-pointer-check --no-bounds-check --no-assertions --termination --max-inductive-step 3
// MODE: FUTURE
// EXPECT: ^Recurrent state shows a non-terminating execution.*$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
