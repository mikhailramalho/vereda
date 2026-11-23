int main(int argc, char *argv[])
{
  if (argc > 1)
    ;
}
// Imported from ESBMC regression/esbmc/github_2639
// Needs: --tuple-sym-flattener --smt-symex-guard
// RUN: --tuple-sym-flattener --smt-symex-guard --z3
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
