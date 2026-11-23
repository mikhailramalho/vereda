int main()
{
  return 0;
}
// Imported from ESBMC regression/esbmc/cmdline_repeated_option_fail
// RUN: --unwind 2 --unwind 2
// EXPECT: Invalid command line: option .--unwind. cannot be specified more than once
// Vereda: unmet EXPECT: Invalid command line: option .--unwind. cannot be specified more than once
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
