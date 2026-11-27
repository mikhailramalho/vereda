int main()
{
L:
  int a = 1, b = 2;
  int *p = &b;
  __VERIFIER_assert(*p == 2); // b is still alive after its labelled declaration
  return a;
}
// Imported from ESBMC regression/esbmc/label_multi_decl_scope
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
