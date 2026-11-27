// InstCombine deletes y's unused select and leaves y's debug record poison:
// the counterexample shows no assignment to y rather than an arbitrary value.
// EXPECT: ^  x = 5;
// EXPECT-NOT: ^  y =
// EXPECT: ^  FAILED       \[main\.assertion\.1\]  line 13  __VERIFIER_assert$
// EXPECT: ^VERIFICATION FAILED$
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);
int main(void) {
  int x = __VERIFIER_nondet_int();
  int y = x > 0 ? x : 7;
  (void)y;
  __VERIFIER_assert(x != 5);
  return 0;
}
