// --witness writes nothing when no assertion fails.
// RUN: --witness -
// EXPECT-NOT: entry_type
// EXPECT: VERIFICATION SUCCESSFUL
extern void __VERIFIER_assert(int);
extern int __VERIFIER_nondet_int(void);
int main(void) {
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assert(x * 0 == 0);
  return 0;
}
