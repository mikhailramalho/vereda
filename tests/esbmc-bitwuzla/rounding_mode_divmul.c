int __VERIFIER_nondet_int(void);

/* A 12-bit x % m spelled as x - (x / m) * m. Bitwuzla cannot settle it by
 * propagation, so CaDiCaL runs a local-search round, and that round sizes its
 * score table with a loop that never ends if the FPU rounds upward. */
int main(void)
{
  _BitInt(12) x = (_BitInt(12))__VERIFIER_nondet_int();
  _BitInt(12) m = (_BitInt(12))__VERIFIER_nondet_int();
  __VERIFIER_assume(x >= 0 && m > 0);
  _BitInt(12) r = x - (x / m) * m;
  __VERIFIER_assert(r >= 0); // remainder is not negative
  return 0;
}
// Imported from ESBMC regression/bitwuzla/rounding_mode_divmul
// RUN: --solver bitwuzla --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
