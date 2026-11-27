void foo(char **q)
{
  q[2];
}

int main(int argc, char **argv)
{
  char *b[2+1];
  foo(b);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_178_constant
// EXPECT: ^VERIFICATION SUCCESSFUL$
