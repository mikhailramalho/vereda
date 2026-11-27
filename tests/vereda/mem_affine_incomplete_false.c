// Unwritten bytes must not be inferred from a progression in the written prefix.
// RUN: --no-summarize-array-loops --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
int main(void) {
  unsigned a[256];
  for (unsigned i = 0; i < 255; ++i)
    a[i] = 5 + 3 * i;
  unsigned k = __VERIFIER_nondet_uint();
  __VERIFIER_assume(k < 256);
  __VERIFIER_assert(a[k] == 5 + 3 * k);
}
