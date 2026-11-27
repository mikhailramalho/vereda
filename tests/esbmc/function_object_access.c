int f(void) { return 7; }
int main()
{
  int (*p)(void) = f;
  void *v = (void *)f;
  int x = 0;
  __VERIFIER_assert(p() == 7); // call through pointer
  __VERIFIER_assert(v == (void *)&f); // address compare
  *(int *)&x = 1;
  __VERIFIER_assert(x == 1); // data write
  return 0;
}
// Imported from ESBMC regression/esbmc/function_object_access
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
