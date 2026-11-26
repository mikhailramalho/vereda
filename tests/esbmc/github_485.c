int main()
{
  int resp, a, b;

  resp=a+b;

  assert(resp==a+b);
}
// Imported from ESBMC regression/esbmc/github_485
// RUN: --overflow-check
// EXPECT: ^VERIFICATION FAILED$
