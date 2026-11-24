extern void __VERIFIER_assume(_Bool);
extern _Bool __ESBMC_forall(void *, _Bool);
extern _Bool __ESBMC_exists(void *, _Bool);

int main()
{
  unsigned n;
  int arr[n];
  unsigned i;

  __VERIFIER_assume(n > 1 && n < 10);

  int array_is_sorted =
    __ESBMC_forall(&i, !(i >= 0 && i < (n - 1)) || arr[i] <= arr[i + 1]);

  // Assume sorted order
  __VERIFIER_assume(array_is_sorted);

  // Ensure last element is the largest
  for (unsigned j; j < n - 1; j++)
    __VERIFIER_assert(arr[j] <= arr[j + 1]); // Array is sorted

  return 0;
}
// Imported from ESBMC regression/cvc/quantifiers-sort
// Needs: __ESBMC_forall
// RUN: --solver cvc5 --incremental-bmc
// MODE: KNOWNBUG
// EXPECT: ^VERIFICATION SUCCESSFUL$
