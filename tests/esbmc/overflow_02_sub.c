int main()
{
  int resp, a, b;

  resp=a-b;

  assert(resp==a-b);
}
// Imported from ESBMC regression/esbmc/overflow_02_sub
// RUN: --overflow-check
// EXPECT: ^VERIFICATION FAILED$
