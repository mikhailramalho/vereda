int main()
{
  return 0;
}
// Imported from ESBMC regression/esbmc/ppc_macos_unknown_target
// Needs: --ppc-macos
// RUN: --ppc-macos
// MODE: FUTURE
// EXPECT: unknown target triple
// EXPECT: ^ERROR: PARSING ERROR$
// Vereda: verdict none
