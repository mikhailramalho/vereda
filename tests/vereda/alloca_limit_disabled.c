// RUN: --no-vla-size-check
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT-NOT: alloca: size exceeds PTRDIFF_MAX
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);
int main(void) {
  if (__VERIFIER_nondet_int()) {
    volatile char *p = __builtin_alloca(-1);
    *p = 1;
    __VERIFIER_assert(0);
  }
}
