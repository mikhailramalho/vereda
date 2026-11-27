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
// Needs: --force-realloc-success --termination --max-inductive-step 3
// RUN: --ub-div-by-zero-check=false --force-malloc-success --force-realloc-success --no-align-check --k-step 2 --vla-size-check=false -m32 --ub-pointer-check=false --no-bounds-check --no-assertions --termination --max-inductive-step 3
// EXPECT: ^Recurrent state shows a non-terminating execution.*$
// EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict none
