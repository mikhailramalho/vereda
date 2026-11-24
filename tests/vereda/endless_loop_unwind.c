// An endless loop that does nothing has no unwinding assertion to fail, nor
// has a loop inside it that ends: nothing after either is reached.
// RUN: --unwind 3
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT-NOT: unwinding assertion
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);

int main(void) {
  int x = __VERIFIER_nondet_int(), n = __VERIFIER_nondet_int();
  if (x > 0)
    while (1)
      ;
  if (x == 0)
    while (1)
      for (int i = 0; i < n; i++)
        ;
  __VERIFIER_assert(x < 0);
  return 0;
}
