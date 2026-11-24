// A loop whose start and count come from a loop before it that is unrolled
// keeps the count the claim gets wrong, reached on the third try.
// RUN: --unwind 3 --no-unwinding-assertions
// EXPECT: ^Summarised 1 array loop\(s\)$
// EXPECT: ^VERIFICATION FAILED$
extern void __VERIFIER_assert(int);
extern unsigned __VERIFIER_nondet_uint(void);
int main(void) {
  unsigned tries = 0, x;
  while (1) {
    x = __VERIFIER_nondet_uint();
    if (x < 100)
      break;
    tries++;
  }
  unsigned y = tries;
  for (unsigned i = 0; i < x; i++)
    y++;
  __VERIFIER_assert(y != 101);
  return 0;
}
