// #9222: an unsigned product above UINT_MAX is an overflow.
unsigned __VERIFIER_nondet_uint(void);
int main(void)
{
  unsigned u = __VERIFIER_nondet_uint(), v = __VERIFIER_nondet_uint();
  __VERIFIER_assume(u <= 65536 && v <= 65536);
  return (int)(u * v);
}
// Imported from ESBMC regression/z3/github_9222_unsigned_fail
// RUN: --solver z3 --unsigned-overflow-check
// EXPECT: ^\s+u = 65536
// EXPECT: ^\s+v = 65536
// EXPECT: arithmetic overflow on mul
// EXPECT: ^VERIFICATION FAILED$
