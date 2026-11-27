// An odd second difference survives both byte wrap and indices beyond 255.
// RUN: --no-summarize-array-loops --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
int main(void) {
  unsigned char a[1024];
  for (unsigned i = 0; i < 1024; ++i)
    a[i] = (unsigned char)(7 + 3 * i + i * (i - 1) / 2);
  unsigned k = __VERIFIER_nondet_uint();
  __VERIFIER_assume(k < 1024);
  __VERIFIER_assert(a[k] == (unsigned char)(7 + 3 * k + k * (k - 1) / 2));
}
