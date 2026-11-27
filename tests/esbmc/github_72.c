// #72: a write through a pointer that may not point anywhere valid must not
// emit an SSA assignment to the failed-dereference symbol.
int __VERIFIER_nondet_int(void);
int x;
int main(void)
{
  int *p;
  if (__VERIFIER_nondet_int())
    p = &x;
  *p = 1;
  __VERIFIER_assert(x == 0 || x == 1); // x keeps a defined value
  return 0;
}
// Imported from ESBMC regression/esbmc/github_72
// RUN: --ub-pointer-check=false
// EXPECT: \A(?![\s\S]*invalid_object\d*&\d+#\d+ ==)
// EXPECT: ^VERIFICATION SUCCESSFUL$
