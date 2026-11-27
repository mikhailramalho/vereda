// Symbolic bases, commuted additions and point overrides keep their exact values.
// RUN: --no-summarize-array-loops --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
int main(void) {
  unsigned x = __VERIFIER_nondet_uint(), special = __VERIFIER_nondet_uint();
  unsigned a[256];
  for (unsigned i = 0; i < 256; ++i)
    a[i] = x + 7 * i;
  a[0] = special;
  a[100] += 1;
  unsigned k = __VERIFIER_nondet_uint();
  __VERIFIER_assume(k < 256);
  __VERIFIER_assert(a[k] == (k == 0 ? special : x + 7 * k + (k == 100)));
}
