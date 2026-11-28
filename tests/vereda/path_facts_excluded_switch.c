// RUN: --dump-smt-only
// EXPECT: ^\(assert false\)\n\(check-sat\)
// The default of a switch excludes each case value from x; after the
// join with a branch that excludes them too, x is none of them (#367).
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);
int main(void) {
  int x = __VERIFIER_nondet_int();
  int c = __VERIFIER_nondet_int();
  if (c) {
    switch (x) {
    case 3: return 0;
    case 5: return 1;
    case 9: return 2;
    default: break;
    }
  } else {
    if (x == 3 || x == 5 || x == 9)
      return 3;
  }
  __VERIFIER_assert(x != 5);
  return 4;
}
