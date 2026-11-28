// EXPECT: ^VERIFICATION FAILED$
// The branches exclude different values, 1 and 2, so after the join x may
// be either: x = 1 when c is 0 reaches the assertion (#367).
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);
int main(void) {
  int x = __VERIFIER_nondet_int();
  int c = __VERIFIER_nondet_int();
  if (c) {
    if (x == 1)
      return 0;
  } else {
    if (x == 2)
      return 1;
  }
  __VERIFIER_assert(x != 1);
  return 3;
}
