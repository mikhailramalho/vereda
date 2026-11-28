// Half and fp128 nondet values get exact hexadecimal constants in the
// violation witness, as float, double and long double do: no suffix for a
// half, Q for an fp128, whose 112 fraction bits a double cannot hold.
// RUN: --witness - -m64
// EXPECT: value: "\\\\result == 0x1\.8p0"(\n.*){3}\n +line: 15\n
// EXPECT: value: "\\\\result == 0x1\.0000000000000000000000000001p0Q"(\n.*){3}\n +line: 16\n
// EXPECT: value: "\\\\result > 0x1\.ffcp15"(\n.*){3}\n +line: 17\n
// EXPECT: VERIFICATION FAILED
extern void __assert_fail(const char *, const char *, unsigned, const char *);
void reach_error(void) { __assert_fail("0", "witness_half_fp128.c", 3, "reach_error"); }
extern _Float16 __VERIFIER_nondet_half(void);
extern __float128 __VERIFIER_nondet_float128(void);
int main(void) {
  // Each call has a line of its own so that its waypoint shows by its line.
  _Float16 h = __VERIFIER_nondet_half();
  __float128 q = __VERIFIER_nondet_float128();
  _Float16 inf = __VERIFIER_nondet_half();
  if (h == 1.5f16 && q == 1 + 0x1p-112Q && inf == (_Float16)__builtin_inff())
    reach_error();
  return 0;
}
