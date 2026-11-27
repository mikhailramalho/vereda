int main()
{
  short a;
  int b=__VERIFIER_nondet_int();

  a = (short)b;
}
// Imported from ESBMC regression/esbmc/overflow_19_typecast
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
