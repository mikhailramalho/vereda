int main()
{
  int resp, a;

  resp=-a;

  assert(resp==-a);
}
// Imported from ESBMC regression/esbmc/overflow_04_unary
// RUN: --overflow-check
// EXPECT: ^VERIFICATION FAILED$
