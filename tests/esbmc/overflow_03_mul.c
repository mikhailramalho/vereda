int main()
{
  int resp, a, b, c, d, resp1;

  resp=a*b;
  resp1=c*d;

  assert(resp==a*b);
}
// Imported from ESBMC regression/esbmc/overflow_03_mul
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
