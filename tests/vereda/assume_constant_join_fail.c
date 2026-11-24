// EXPECT: ^VERIFICATION FAILED$
// x == 5 holds only on the path through the assumption; the join after the
// branch also has the path that skipped it, so x stays x there (#120).
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
int main(void) {
  int x = __VERIFIER_nondet_int();
  if (__VERIFIER_nondet_int())
    __VERIFIER_assume(x == 5);
  __VERIFIER_assert(x * 2 == 10);
  return 0;
}
