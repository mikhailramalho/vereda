int main()
{
  int resp, a, b;

  resp=a+b;

  assert(resp==a+b);
}
// Imported from ESBMC regression/esbmc/overflow_01_addition
// RUN: --overflow-check
// EXPECT: ^VERIFICATION FAILED$
