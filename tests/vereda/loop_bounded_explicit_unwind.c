// RUN: --unwind 5
// EXPECT: unwinding assertion
// EXPECT: ^VERIFICATION FAILED$
// Bounded-unroll must not bypass an explicit --unwind N requested by the user (#253).
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);
int main(void) {
  int x = 1, y = 0;
  while (y < 100 && __VERIFIER_nondet_int()) {
    x = x + y;
    y = y + 1;
  }
  __VERIFIER_assert(x >= y);
  return 0;
}
