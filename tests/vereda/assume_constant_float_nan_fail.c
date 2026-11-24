// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  SUCCESS +\[main\.assertion\.1\] +line 12 
// EXPECT: ^  FAILED +\[main\.assertion\.2\] +line 15 
// x != 1.0 holds for a NaN, so x == x may fail after it; y == 2.0 rules a
// NaN out, and the first assertion holds (#120).
extern float __VERIFIER_nondet_float(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
int main(void) {
  float y = __VERIFIER_nondet_float();
  __VERIFIER_assume(y == 2.0f);
  __VERIFIER_assert(y == y && y * 2.0f == 4.0f);
  float x = __VERIFIER_nondet_float();
  __VERIFIER_assume(!(x == 1.0f));
  __VERIFIER_assert(x == x);
  return 0;
}
