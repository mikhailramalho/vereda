// An endless loop that may reach __builtin_unreachable, a check, has a way
// out, so it is unrolled and the check fails.
// RUN: --unwind 3 --no-unwinding-assertions
// EXPECT: __builtin_unreachable
// EXPECT: ^VERIFICATION FAILED$
extern int __VERIFIER_nondet_int(void);

int main(void) {
  int x = __VERIFIER_nondet_int();
  while (1)
    if (x)
      __builtin_unreachable();
}
