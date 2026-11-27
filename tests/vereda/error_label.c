// With --error-label ERROR, a label ERROR no run reaches is no violation,
// and another label, ERR here, is not checked though every run reaches it.
// RUN: --error-label ERROR
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern int __VERIFIER_nondet_int(void);
int main(void) {
  int x = __VERIFIER_nondet_int();
  int y = x;
ERR:
  if (x != y) {
  ERROR:
    return 1;
  }
  return 0;
}