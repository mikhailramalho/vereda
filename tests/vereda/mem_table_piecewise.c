// Contiguous segments may use distinct symbolic bases and constant values.
// RUN: --no-summarize-array-loops
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
int main(void) {
  unsigned x = __VERIFIER_nondet_uint(), y = __VERIFIER_nondet_uint();
  unsigned a[256];
  for (unsigned i = 0; i < 256; ++i)
    a[i] = i < 64 ? x + 3 * i : i < 192 ? y - 5 * (i - 64) : x;
  unsigned k = __VERIFIER_nondet_uint();
  __VERIFIER_assume(k < 256);
  __VERIFIER_assert(a[k] == (k < 64 ? x + 3 * k : k < 192 ? y - 5 * (k - 64) : x));
}
