// Failing assertion on symbolic long double (x86_fp80) in default and --fp2bv modes.
// RUN: -m64
// RUN: -m64 --fp2bv
// EXPECT: ^VERIFICATION FAILED$
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
extern long double __VERIFIER_nondet_long_double(void);

int main(void) {
  long double x = __VERIFIER_nondet_long_double();
  __VERIFIER_assume(x > 1.0L && x < 2.0L);
  __VERIFIER_assert(x * 2.0L < 3.0L);
  return 0;
}
