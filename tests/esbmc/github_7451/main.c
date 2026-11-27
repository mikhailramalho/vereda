int global = 1234;
int combine(int a, int b)
{
  return a - b;
}
// Imported from ESBMC regression/esbmc/github_7451
// FILES: weak.c
// EXPECT: ^VERIFICATION SUCCESSFUL$
