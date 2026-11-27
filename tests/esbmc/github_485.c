int main()
{
  int resp, a, b;

  resp=a+b;

  assert(resp==a+b);
}
// Imported from ESBMC regression/esbmc/github_485
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
