// A symbolic read from an array of int elements (#107) finds the element a
// loop wrote that breaks the assertion. The loop writes 6000 bytes at
// constant indices, so the known bytes go to the array mid-loop.
// RUN: --unwind 1501
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: line 16
extern void __VERIFIER_assert(int);
extern void __VERIFIER_assume(int);
extern unsigned __VERIFIER_nondet_uint(void);
int a[2048];
int main(void) {
  for (int k = 0; k < 1500; k++)
    a[k] = 3 * k;
  unsigned x = __VERIFIER_nondet_uint();
  __VERIFIER_assume(x < 2048);
  __VERIFIER_assert(a[x] != 2997);
  return 0;
}
