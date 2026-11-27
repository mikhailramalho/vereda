// RUN: --ub-shift-check --ub-float-cast-overflow-check --overflow-check --multi-property
// EXPECT: ^  FAILED .* line 19  undefined behavior on shift operation shl$
// EXPECT: ^  FAILED .* line 20  floating-point conversion out of range of signedbv on typecast$
// EXPECT: ^  FAILED .* line 21  division by zero$
// EXPECT: ^  FAILED .* line 22  arithmetic overflow on sub$
// EXPECT: ^VERIFICATION FAILED$
// Under --multi-property a run goes on past failed shift, float conversion
// and division checks too: every check below fails on every path, a - 1
// only when a is INT_MIN, so each is reported only if the run gets past
// the ones before it.
extern int __VERIFIER_nondet_int(void);
extern double __VERIFIER_nondet_double(void);
extern void __VERIFIER_assume(int);
int main(void) {
  int a = __VERIFIER_nondet_int(), b = __VERIFIER_nondet_int();
  double d = __VERIFIER_nondet_double();
  __VERIFIER_assume(a < 0 && d > 1e10);
  __VERIFIER_assume(b == 0);
  int x = a << 1;
  int y = (int)d;
  int z = 100 / b;
  int c = a - 1;
  return x == y && z == c;
}
