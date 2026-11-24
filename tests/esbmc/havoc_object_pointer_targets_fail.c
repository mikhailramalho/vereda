void __ESBMC_havoc_object(void *);
int a, b;
int main(void)
{
  a = 1;
  b = 2;
  int *p = __VERIFIER_nondet_int() ? &a : &b;
  __ESBMC_havoc_object(p);
  if (p == &b)
    __VERIFIER_assert(b == 2); // the object p points to is havocked
  return 0;
}
// Imported from ESBMC regression/esbmc/havoc_object_pointer_targets_fail
// Needs: __ESBMC_havoc_object
// RUN: --incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
