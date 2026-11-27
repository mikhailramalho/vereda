/* argv[0] .. argv[argc-1] point to strings (C11 5.1.2.2.1), so reading them is
   not a dereference failure up to --argv-max-args, 5 by default
   (esbmc/esbmc#10239). */
int main(int argc, char **argv)
{
  if (argc == 5)
    return argv[2][0] + argv[4][0];
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10239
// RUN: --multi-property --no-incremental-bmc
// EXPECT: ^\s*PASSED\s+\[main\.null-pointer-dereference\.1\]
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^\s*PASSED\s+\[main\.null-pointer-dereference\.1\]
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
