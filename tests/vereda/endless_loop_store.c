// An endless loop that only stores, even where the passes keep the store
// (volatile g, *p), ends the paths into it as an empty one does: nothing
// after it is reached, so it has no unwinding assertion to fail (#373).
// RUN: --unwind 3
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT-NOT: unwinding assertion
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);
volatile int g;
int h;

int main(void) {
  int x = __VERIFIER_nondet_int();
  int *p = x > 5 ? &h : (int *)&g;
  if (x > 0)
    for (;;)
      g++;
  if (x == 0)
    for (;;)
      *p = 1;
  __VERIFIER_assert(x < 0);
  return 0;
}
