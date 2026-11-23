/* The addresses of two differently named objects, declared or allocated,
 * are never the same object, so each claim below folds in the simplifier
 * and none reaches the solver. */
int a, b;
int arr[4];
struct pair
{
  int x, y;
} s;

int main()
{
  long *p = __builtin_alloca(sizeof(long));
  long *q = __builtin_alloca(sizeof(long));
  __VERIFIER_assert(!__ESBMC_same_object(&a, &b)); // two globals
  __VERIFIER_assert(!__ESBMC_same_object(&arr[1], &b)); // element and global
  __VERIFIER_assert(!__ESBMC_same_object(&s.y, &arr[2])); // member and element
  __VERIFIER_assert(!__ESBMC_same_object(p, q)); // two allocations
  __VERIFIER_assert(__ESBMC_same_object(&arr[0], &arr[3])); // one array
  return 0;
}
// Imported from ESBMC regression/esbmc/same_object_distinct_fold
// EXPECT: ^Generated \d+ VCC\(s\), 0 remaining after simplification \(\d+ assignments\)$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^Generated \d+ VCC\(s\), 0 remaining after simplification \(\d+ assignments\)$
// MODE: FUTURE
