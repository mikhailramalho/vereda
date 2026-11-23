// The uninitialised-variable check would read the summary's unwritten
// elements, so the pass stays off under it.
int main()
{
  int a[4];
  for (int i = 0; i < 4; i++)
    a[i] = 0;
  __VERIFIER_assert(a[2] == 0); // a[2]
}
// Imported from ESBMC regression/esbmc/array_loop_summary_uninit
// Needs: --summarize-array-loops --uninitialised-vars-check
// RUN: --summarize-array-loops --uninitialised-vars-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
