// A pointer element the model resolves is reported as itself: an address and a
// null pointer are values the model produced, not expressions symex propagated.
int x = 5;
int *arr[2];

int main(void)
{
  arr[0] = &x;
  arr[1] = 0;
  __VERIFIER_assert(arr[0] != &x); // arr0 is &x
  return 0;
}
// Imported from ESBMC regression/esbmc/counterexample_pointer_component_fail
// EXPECT: ^  arr\[0\] = &x;$
// EXPECT: ^  arr\[1\] = 0;$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^  arr\[0\] = &x$
// MODE: FUTURE
// Vereda: verdict FAILED
