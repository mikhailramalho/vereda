// The failing twin of mem_constant_stores: the store at a symbolic index may
// overwrite arr[3].
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
  __VERIFIER_assert(*p == 5 && arr[3] == 7);
  return 0;
}
// EXPECT: ^VERIFICATION FAILED$
