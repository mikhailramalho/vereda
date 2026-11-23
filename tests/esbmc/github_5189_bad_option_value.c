// A non-numeric argument to a numeric option makes boost::program_options
// throw. Before #5189 that exception escaped main and hit std::terminate, so
// the run aborted with no ESBMC diagnostic at all -- which is also what made
// the CI-only crash this test's issue reports impossible to attribute.
int main(void)
{
  return 0;
}
// Imported from ESBMC regression/esbmc/github_5189_bad_option_value
// RUN: --unwind abc
// EXPECT: ^ERROR: Invalid command line: the argument .+for option .--unwind. is invalid
// Vereda: unmet EXPECT: ^ERROR: Invalid command line: the argument .+for option .--unwind. is invalid
// MODE: FUTURE
// Vereda: verdict none
