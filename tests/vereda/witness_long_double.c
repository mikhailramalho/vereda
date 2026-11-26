// A long double (x86_fp80) nondet value gets an exact hexadecimal constant
// with an L suffix in the violation witness.
// RUN: --witness - -m64
// EXPECT: value: "\\\\result == 0x1\.8p0L"
// EXPECT: VERIFICATION FAILED
extern void __assert_fail(const char *, const char *, unsigned, const char *);
void reach_error(void) { __assert_fail("0", "witness_long_double.c", 3, "reach_error"); }
extern long double __VERIFIER_nondet_long_double(void);
int main(void) {
  long double x = __VERIFIER_nondet_long_double();
  if (x == 1.5L)
    reach_error();
  return 0;
}
