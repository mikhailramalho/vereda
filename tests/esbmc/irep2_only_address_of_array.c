/* `&Q` on an array is `&Q[0]`: the pointer designates the first element, not
   the array object. */
int Q[3];
int *b = (int *)&Q;

int main(void)
{
  Q[0] = 7;
  return b[0];
}
// Imported from ESBMC regression/esbmc/irep2_only_address_of_array
// Needs: --goto-functions-only --clang-c-irep2-adjust-only
// RUN: --goto-functions-only --clang-c-irep2-adjust-only
// MODE: FUTURE
// EXPECT: &Q\[0\]
// Vereda: verdict none
