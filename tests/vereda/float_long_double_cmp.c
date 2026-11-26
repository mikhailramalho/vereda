// Comparing symbolic long doubles (x86_fp80) in default and --fp2bv modes.
// RUN: -m64
// RUN: -m64 --fp2bv
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
extern long double a, b;
int main(void) {
  __VERIFIER_assume(a == 1.0L && b == 2.0L);
  __VERIFIER_assert(a < b);
  return 0;
}
