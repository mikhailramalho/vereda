// RUN: --error-label ERROR --multi-property
// EXPECT: ^  FAILED       \[main\.assertion\.1\]  line 12  error label$
// EXPECT: ^  FAILED       \[main\.assertion\.2\]  line 15  __VERIFIER_assert$
// Under --multi-property a run goes on past an error label, as past a
// failed assertion: the assertion after it fails too, only when x is 42.
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);
int main(void) {
  int x = __VERIFIER_nondet_int();
  int y = 0;
  if (x == 42) {
  ERROR:
    y = 1;
  }
  __VERIFIER_assert(y == 0);
  return 0;
}
