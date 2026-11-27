int main()
{
  return 0;
}
// Imported from ESBMC regression/esbmc/default_solver_not_built
// Needs: --default-solver nosuchsolver
// RUN: --default-solver nosuchsolver --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^ERROR: The nosuchsolver solver has not been built into this version of ESBMC, sorry$
// Vereda: verdict none
