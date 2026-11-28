// RUN: --dump-smt-only
// EXPECT: ^\(assert false\)\n\(check-sat\)
// x and y lie in [0x7ffffff8, 0x80000008], across the sign bit, so only
// their unsigned bounds are tight. x is not 0x7ffffff9, so x != 0x7ffffff8
// moves its lower end to 0x7ffffff9, which is excluded, and on to
// 0x7ffffffa; y is not 0x80000007, so y != 0x80000008 moves its upper end
// to 0x80000007 and on to 0x80000006. Both checks fold to false as the
// walk builds them (#467).
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assert(int);
int main(void) {
  unsigned x = __VERIFIER_nondet_uint();
  unsigned y = __VERIFIER_nondet_uint();
  if (x < 0x7ffffff8u || x > 0x80000008u || y < 0x7ffffff8u || y > 0x80000008u)
    return 0;
  if (x == 0x7ffffff9u || y == 0x80000007u)
    return 1;
  if (x == 0x7ffffff8u || y == 0x80000008u)
    return 2;
  __VERIFIER_assert(x >= 0x7ffffffau);
  __VERIFIER_assert(y <= 0x80000006u);
  return 3;
}
