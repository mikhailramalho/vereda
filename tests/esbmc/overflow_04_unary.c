int main()
{
  int resp, a;

  resp=-a;

  assert(resp==-a);
}
// Imported from ESBMC regression/esbmc/overflow_04_unary
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
