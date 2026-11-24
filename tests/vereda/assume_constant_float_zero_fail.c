// RUN: --multi-property
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  FAILED +\[main\.assertion\.1\]
// EXPECT: ^  FAILED +\[main\.assertion\.2\]
// x == 0.0 holds for -0.0 too, so the assumption does not make x the
// constant +0.0: its sign stays open (#120; ESBMC had this bug).
extern float __VERIFIER_nondet_float(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
int main(void) {
  float x = __VERIFIER_nondet_float();
  __VERIFIER_assume(x == 0.0f);
  __VERIFIER_assert(1.0f / x > 0.0f);
  __VERIFIER_assert(!__builtin_signbit(x));
  return 0;
}
