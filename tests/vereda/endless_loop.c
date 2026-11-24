// A loop with no way out that does nothing ends the paths into it, as
// ESBMC's assume(0), instead of being unrolled forever. Clang leaves an
// edge out of do-while (1) on a constant, never taken.
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT-NOT: Unwinding loop
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);

int main(void) {
  int x = __VERIFIER_nondet_int();
  if (x > 0) {
    do {
    } while (1);
    __VERIFIER_assert(0);
  }
  if (x == 0)
    while (1)
      ;
  __VERIFIER_assert(x < 0);
  return 0;
}
