/* One past argv's last valid index. The counterpart of argc_argv_bounds:
   without both halves neither pins the size (esbmc/esbmc#4715). */
int main(int argc, char **argv)
{
  return argv[argc + 1] == 0 ? 0 : 1;
}
// Imported from ESBMC regression/esbmc/argc_argv_bounds_fail
// RUN: --multi-property --no-incremental-bmc
// EXPECT: ^\s*FAILED\s+\[main\.array-bounds-violated\.1\]
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^\s*FAILED\s+\[main\.array-bounds-violated\.1\]
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
