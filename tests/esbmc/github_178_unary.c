void foo(char **q)
{
  q[5];
}

int main(int argc, char **argv)
{
  int a;
  __VERIFIER_assume(a >= 5 && a <= 20);
  char *b[++a];
  foo(b);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_178_unary
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
