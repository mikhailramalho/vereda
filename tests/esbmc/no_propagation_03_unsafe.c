int main()
{
  int a[4];
  int i = 3;
  int j = i + 1;
  a[j] = 0;
  return 0;
}
// Imported from ESBMC regression/esbmc/no_propagation_03_unsafe
// Needs: --no-propagation
// RUN: --no-propagation
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
