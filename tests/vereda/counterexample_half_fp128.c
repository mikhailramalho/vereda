// Half (_Float16) and fp128 (__float128) values in the counterexample, as
// float, double and long double are: %.6e's digits with the exponent
// unpadded, +INFINITY by the sign bit, the bits in the comment, no suffix for
// a half and Q for an fp128. htiny and qtiny are subnormal (qtiny below the
// range of x86's long double too); ha, qa and la are global arrays, so their
// assignments are stores.
// RUN: -m64
// RUN: -m64 --fp2bv
// EXPECT: ^  h = 2\.500000e\+0; // 01000001 00000000$
// EXPECT: ^  hinf = \+INFINITY; // 01111100 00000000$
// EXPECT: ^  htiny = 2\.980232e\-7; // 00000000 00000101$
// EXPECT: ^  ha\[1\] = \-3\.125000e\-1; // 10110101 00000000$
// EXPECT: ^  q = 2\.500000e\+0Q; // 01000000 00000000 01000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000$
// EXPECT: ^  qbig = 2\.500000e\+4000Q; // 01110011 11101000 00000110 00101001 00100011 11101100 11111101 11101010 11101111 01111001 01010000 10110011 10100011 01001011 00100111 11111010$
// EXPECT: ^  qinf = \+INFINITY; // 01111111 11111111 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000$
// EXPECT: ^  qtiny = 2\.500000e\-4960Q; // 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000101 11100100 00101010$
// EXPECT: ^  qa\[1\] = \-3\.125000e\-1Q; // 10111111 11111101 01000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000$
// EXPECT: ^  la\[1\] = 2\.500000e\+0L; // 01000000 00000000 10100000 00000000 00000000 00000000 00000000 00000000 00000000 00000000$
// EXPECT: ^VERIFICATION FAILED$
extern float __VERIFIER_nondet_float(void);
extern double __VERIFIER_nondet_double(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
_Float16 ha[2];
__float128 qa[2];
long double la[2];
int main(void) {
  _Float16 h = __VERIFIER_nondet_float();
  __VERIFIER_assume(h == 2.5f16);
  _Float16 hinf = h * 30000;
  _Float16 htiny = h * 1e-7f16;
  ha[1] = -h / 8;
  __float128 q = __VERIFIER_nondet_double();
  __VERIFIER_assume(q == 2.5Q);
  __float128 qbig = q * 1e4000Q;
  __float128 qinf = qbig * 1e1000Q;
  __float128 qtiny = q * 1e-4960Q;
  qa[1] = -q / 8;
  la[1] = h;
  __VERIFIER_assert(hinf != hinf || htiny == 0 || qinf != qinf || qtiny == 0);
  return 0;
}
