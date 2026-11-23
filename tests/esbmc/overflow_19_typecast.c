int main()
{
  short a;
  int b=__VERIFIER_nondet_int();

  a = (short)b;
}
// Imported from ESBMC regression/esbmc/overflow_19_typecast
// RUN: --overflow-check
// EXPECT: ^VERIFICATION SUCCESSFUL$
