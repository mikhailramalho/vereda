/* A pointer that may hold either address is not folded: the solver still
 * finds the path where it points at a. */
int a, b;

int main()
{
  int *p = __VERIFIER_nondet_bool() ? &a : &b;
  __VERIFIER_assert(!__ESBMC_same_object(p, &a)); // p may point at a
  return 0;
}
// Imported from ESBMC regression/esbmc/same_object_distinct_fold_fail
// Needs: __ESBMC_same_object
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
