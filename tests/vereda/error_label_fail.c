// With --error-label ERROR, reaching the label ERROR is a violation,
// reported as ESBMC reports an error label; here only when x is 42.
// RUN: --error-label ERROR
// EXPECT: ^  FAILED       \[f\.assertion\.1\]  line 11  error label$
// EXPECT: ^VERIFICATION FAILED$
extern int __VERIFIER_nondet_int(void);
static int f(int x) {
ERR:
  x++;
  if (x == 43) {
  ERROR:
    return 1;
  }
  return 0;
}
int main(void) { return f(__VERIFIER_nondet_int()); }