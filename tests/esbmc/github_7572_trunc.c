/* C11 6.3.1.4p1 constrains the truncated integral part, not the value, so a
   magnitude below one always converts and MIN - 0.5 converts to MIN. The first
   revision of the #7572 check used a closed lower bound and reported both as
   undefined; that cost an SV-COMP incorrect-false on float-benchs/bary_diverge,
   where a nondet float assumed into [-1, 1] is cast to an enum.
   clang -fsanitize=float-cast-overflow accepts every conversion below. */
extern float __VERIFIER_nondet_float(void);
extern double __VERIFIER_nondet_double(void);

int main(void)
{
  float a = __VERIFIER_nondet_float();
  __VERIFIER_assume(a > -1.0f && a <= 0.0f);
  unsigned int u = (unsigned int)a;
  __VERIFIER_assert(u == 0u); // a magnitude below one truncates to zero

  double d = __VERIFIER_nondet_double();
  __VERIFIER_assume(d == -2147483648.5);
  int i = (int)d;
  __VERIFIER_assert(i == -2147483647 - 1); // INT_MIN - 0.5 truncates to INT_MIN
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7572_trunc
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check --no-incremental-bmc
// EXPECT: ^  PASSED       \[main\.assertion\.1\]\s+line 14  floating-point conversion out of range of unsignedbv on typecast$
// EXPECT: ^  PASSED       \[main\.assertion\.3\]\s+line 19  floating-point conversion out of range of signedbv on typecast$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^  PASSED       \[main\.assertion\.1\]\s+line 14  floating-point conversion out of range of unsignedbv on typecast$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
