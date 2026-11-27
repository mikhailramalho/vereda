/* argv' is sized argc + 1 because the array is NULL-terminated, so the last
   valid index is argc. This pins the size expression as symex consumes it,
   which the symbol-table renderings cannot (esbmc/esbmc#4715). */
int main(int argc, char **argv)
{
  return argv[argc] == 0 ? 0 : 1;
}
// Imported from ESBMC regression/esbmc/argc_argv_bounds
// RUN: --multi-property --no-incremental-bmc
// EXPECT: ^\s*PASSED\s+\[main\.array-bounds-violated\.1\]
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^\s*PASSED\s+\[main\.array-bounds-violated\.1\]
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
