// EXPECT: ^VERIFICATION FAILED$
// x is in [1, 10] and not 3, so x != 1 moves its lower end to 2 only: 2 is
// not excluded, and x = 2 reaches the assertion (#467).
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assert(int);
int main(void) {
  unsigned x = __VERIFIER_nondet_uint();
  if (x < 1 || x > 10)
    return 0;
  if (x == 3)
    return 1;
  if (x == 1)
    return 2;
  __VERIFIER_assert(x >= 3);
  return 3;
}
