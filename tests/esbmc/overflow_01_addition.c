int main()
{
  int resp, a, b;

  resp=a+b;

  assert(resp==a+b);
}
// Imported from ESBMC regression/esbmc/overflow_01_addition
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
