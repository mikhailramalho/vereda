void foo(char **q)
{
  q[3];
}

int main(int argc, char **argv)
{
  char *b[2+1];
  foo(b);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_178_constant_fail
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
