struct
{
  double a;
  char symbol;
} * b;
int c()
{
  b->a;
}
int main();
// Imported from ESBMC regression/esbmc/github_1071-reduced
// RUN: --entry c --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: \bdereference failure: NULL pointer$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
