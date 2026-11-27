/* Out of bounds through the same comma subscript: the row index must still be
   checked against the array's own bound, not merely as a dereference. */
unsigned g[42][3];
unsigned i;

int main(void)
{
  i = 50;
  if ((i = i, g[i])[0] != 0)
    return 1;
  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_comma_array_type_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: array bounds violated: array `g' upper bound
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
