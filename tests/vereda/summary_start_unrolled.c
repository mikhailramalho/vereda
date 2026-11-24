// A loop whose start and count come from a loop before it that is unrolled:
// SCEV names them by an AddRec of the earlier loop and by the nondet call in
// it, whose own term is the one of the last iteration unrolled. The terms of
// the LCSSA phis that carry them out hold the values each way out leaves.
// RUN: --unwind 3 --no-unwinding-assertions
// EXPECT: ^Summarised 1 array loop\(s\)$
// EXPECT: ^VERIFICATION SUCCESSFUL$
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
  __VERIFIER_assert(y == tries + x);
  return 0;
}
