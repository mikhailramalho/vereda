// A rotate left is not a rotate right, except by 0 or half the width: the
// funnel shifts InstCombine makes of the two must not be encoded alike.
// RUN: -m64
// EXPECT: ^VERIFICATION FAILED$
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assert(int);

int main(void) {
  unsigned a = __VERIFIER_nondet_uint(), s = __VERIFIER_nondet_uint();
  __VERIFIER_assert(__builtin_rotateleft32(a, s) == __builtin_rotateright32(a, s));
  return 0;
}
