// Periodic symbolic values compact without replacing their source symbols.
// RUN: --no-summarize-array-loops --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
int main(void) {
  unsigned x = __VERIFIER_nondet_uint();
  unsigned pattern[8] = {3, 11, 7, 29, 2, 19, 5, 23}, a[256];
  for (unsigned i = 0; i < 256; ++i)
    a[i] = x + pattern[i % 8];
  unsigned k = __VERIFIER_nondet_uint();
  __VERIFIER_assume(k < 256);
  __VERIFIER_assert(a[k] == x + pattern[k % 8]);
}
