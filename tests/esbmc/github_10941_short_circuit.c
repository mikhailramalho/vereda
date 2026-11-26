int __VERIFIER_nondet_int(void);
int a[4];
int main(void)
{
  int x = __VERIFIER_nondet_int();
  int *p = __VERIFIER_nondet_int() ? 0 : &x;
  __VERIFIER_assume(x >= 0 && x < 4);
  /* *p is evaluated only when p is not NULL (C11 6.5.13p4). */
  if (p && a[*p] == 0)
    return 1;
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10941_short_circuit
// EXPECT: ^VERIFICATION SUCCESSFUL$
