// EXPECT: ^VERIFICATION FAILED$
// x is not 6, so !(x <= 4) moves its lower end to 5 only: 5 is not
// excluded, and x = 5 reaches the assertion (#473).
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assert(int);
int main(void) {
  unsigned x = __VERIFIER_nondet_uint();
  if (x == 6)
    return 1;
  if (x <= 4)
    return 2;
  __VERIFIER_assert(x >= 7);
  return 3;
}
