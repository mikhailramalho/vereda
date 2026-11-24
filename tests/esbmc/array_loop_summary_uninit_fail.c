int main()
{
  int a[4];
  for (int i = 0; i < 3; i++)
    a[i] = 0;
  __VERIFIER_assert(a[3] == 0); // a[3]
}
// Imported from ESBMC regression/esbmc/array_loop_summary_uninit_fail
// Needs: --uninitialised-vars-check
// RUN: --uninitialised-vars-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
