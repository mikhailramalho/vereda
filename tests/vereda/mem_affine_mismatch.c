// A progression with a different last element must retain that last value.
// RUN: --no-summarize-array-loops --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
int main(void) {
  unsigned a[256];
  for (unsigned i = 0; i < 256; ++i)
    a[i] = 5 + 3 * i;
  a[255] += 1;
  unsigned k = __VERIFIER_nondet_uint();
  __VERIFIER_assume(k < 256);
  __VERIFIER_assert(a[k] == 5 + 3 * k + (k == 255));
}
