// A symbolic element interrupts the pattern even when every byte was written.
// RUN: --no-summarize-array-loops
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
int main(void) {
  unsigned a[256];
  for (unsigned i = 0; i < 256; ++i)
    a[i] = 5 + 3 * i;
  unsigned special = __VERIFIER_nondet_uint();
  a[255] = special;
  unsigned k = __VERIFIER_nondet_uint();
  __VERIFIER_assume(k < 256);
  __VERIFIER_assert(a[k] == (k == 255 ? special : 5 + 3 * k));
}
