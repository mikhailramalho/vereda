// Float and double values in the counterexample, as ESBMC prints them but
// as C constants: %.6e's digits with the exponent unpadded, an f suffix for
// a float, +INFINITY or -INFINITY, +NAN or -NAN by the sign bit, and the
// bits in the comment. g is a global, so its assignment is a store; tiny
// is a subnormal float.
// RUN:
// RUN: --fp2bv
// EXPECT: ^  d = 2\.500000e\+0; // 01000000 00000100 00000000 00000000 00000000 00000000 00000000 00000000$
// EXPECT: ^  f = -1\.000000e-1f; // 10111101 11001100 11001100 11001101$
// EXPECT: ^  big = 2\.500000e\+300; // 01111110 01001101 11011101 01001011 10101010 00000000 10010011 00000011$
// EXPECT: ^  inf = \+INFINITY; // 01111111 11110000 00000000 00000000 00000000 00000000 00000000 00000000$
// EXPECT: ^  nan = [+-]NAN; // [01]1111111 1111[01]{4}( [01]{8}){6}$
// EXPECT: ^  tiny = -9\.999666e-42f; // 10000000 00000000 00011011 11100000$
// EXPECT: ^  g = 6\.250000e-1; // 00111111 11100100 00000000 00000000 00000000 00000000 00000000 00000000$
// EXPECT: ^VERIFICATION FAILED$
extern double __VERIFIER_nondet_double(void);
extern float __VERIFIER_nondet_float(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
double g;
int main(void) {
  double d = __VERIFIER_nondet_double();
  float f = __VERIFIER_nondet_float();
  __VERIFIER_assume(d == 2.5 && f == -0.1f);
  double big = d * 1e300;
  double inf = big * 1e10;
  double nan = inf - inf;
  float tiny = f * 1e-40f;
  g = d / 4;
  __VERIFIER_assert(nan == nan || tiny == 0.0f);
  return 0;
}
