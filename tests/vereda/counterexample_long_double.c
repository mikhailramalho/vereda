// Long double (x86_fp80) values in the counterexample, printed as float
// and double are but with an L suffix: %.6e's digits with the exponent
// unpadded, +INFINITY by the sign bit, and the 80 bits in the comment. g is
// a global, so its assignment is a store; tiny is subnormal.
// RUN: -m64
// RUN: -m64 --fp2bv
// EXPECT: ^  x = 2\.500000e\+0L; // 01000000 00000000 10100000 00000000 00000000 00000000 00000000 00000000 00000000 00000000$
// EXPECT: ^  big = 2\.500000e\+4000L; // 01110011 11101000 10000011 00010100 10010001 11110110 01111110 11110101 01110111 10111101$
// EXPECT: ^  inf = \+INFINITY; // 01111111 11111111 10000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000$
// EXPECT: ^  tiny = 2\.500000e-4940L; // 00000000 00000000 00000000 00000000 00000000 00001111 11110111 11100010 11100011 11110101$
// EXPECT: ^  g = -3\.125000e-1L; // 10111111 11111101 10100000 00000000 00000000 00000000 00000000 00000000 00000000 00000000$
// EXPECT: ^VERIFICATION FAILED$
extern long double __VERIFIER_nondet_long_double(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
long double g;
int main(void) {
  long double x = __VERIFIER_nondet_long_double();
  __VERIFIER_assume(x == 2.5L);
  long double big = x * 1e4000L;
  long double inf = big * 1e1000L;
  long double tiny = x * 1e-4940L;
  g = -x / 8;
  __VERIFIER_assert(inf != inf || tiny == 0.0L);
  return 0;
}
