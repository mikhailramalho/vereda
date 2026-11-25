// RUN: --multi-property
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: FAILED +\[main\.assertion\.1\].*__VERIFIER_assert
// EXPECT: FAILED +\[main\.assertion\.2\].*alloca: size exceeds PTRDIFF_MAX
// EXPECT: Properties: 2 checked, 0 passed, 2 failed, 0 not checked
extern void __VERIFIER_assert(int);
int main(void) {
  __VERIFIER_assert(0);
  volatile char *p = __builtin_alloca(-1);
  *p = 1;
  __VERIFIER_assert(0);
}
