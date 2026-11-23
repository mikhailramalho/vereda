// Stores at constant addresses go straight into the known bytes without
// building address terms (#97). They must keep a stored pointer's provenance
// and must not lose a store at a symbolic index to the same object.
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
int x = 5;
int *p;
int arr[8];
int main(void) {
  p = &x;
  arr[3] = 7;
  unsigned k = __VERIFIER_nondet_uint();
  __VERIFIER_assume(k < 8);
  arr[k] = 9;
  arr[5] = 11;
  __VERIFIER_assert(*p == 5);
  __VERIFIER_assert(arr[3] == 7 || k == 3);
  __VERIFIER_assert(arr[5] == 11);
  return 0;
}
// EXPECT: ^VERIFICATION SUCCESSFUL$
