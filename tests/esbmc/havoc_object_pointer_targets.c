void __ESBMC_havoc_object(void *);
int a, b;
int main(void)
{
  a = 1;
  b = 2;
  int *p = __VERIFIER_nondet_int() ? &a : &b;
  __ESBMC_havoc_object(p);
  if (p == &a)
    __VERIFIER_assert(b == 2); // b untouched when p points to a
  else
    __VERIFIER_assert(a == 1); // a untouched when p points to b
  return 0;
}
// Imported from ESBMC regression/esbmc/havoc_object_pointer_targets
// Needs: __ESBMC_havoc_object
// RUN: --incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
