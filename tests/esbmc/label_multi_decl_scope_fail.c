int main()
{
L:
  int a = 1, b = 2;
  int *p = &b;
  __VERIFIER_assert(*p == 1); // b holds 2, not 1
  return a;
}
// Imported from ESBMC regression/esbmc/label_multi_decl_scope_fail
// EXPECT: ^VERIFICATION FAILED$
