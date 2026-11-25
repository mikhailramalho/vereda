// A flush where only part of an object is known keeps the known elements intact
// and leaves unwritten elements nondet.
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern void __VERIFIER_assert(int);
extern void __VERIFIER_assume(int);
extern unsigned __VERIFIER_nondet_uint(void);

int main(void) {
  int a[16];
  for (int k = 0; k < 8; ++k)
    a[k] = 5;

  unsigned i = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i < 8);
  __VERIFIER_assert(a[i] == 5);
  return 0;
}
