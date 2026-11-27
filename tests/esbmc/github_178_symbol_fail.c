void foo(char **q)
{
  q[5];
}

int main(int argc, char **argv)
{
  int a;
  __VERIFIER_assume(a >= 5 && a <= 20);
  char *b[a];
  foo(b);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_178_symbol_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
