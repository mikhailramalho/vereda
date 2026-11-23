/* The other side of the open lower bound: -1.0f truncates to -1, which no
   unsigned type represents, so this conversion stays undefined. Pinned so a
   fix for github_7572_trunc cannot be a blanket relaxation.
   clang -fsanitize=float-cast-overflow: "-1 is outside the range of
   representable values of type 'unsigned int'". */
extern float __VERIFIER_nondet_float(void);

int main(void)
{
  float a = __VERIFIER_nondet_float();
  __VERIFIER_assume(a == -1.0f);
  unsigned int u = (unsigned int)a;
  (void)u;
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7572_trunc_fail
// RUN: --overflow-check
// EXPECT: ^  FAILED       \[main\.assertion\.1\]\s+line 12  floating-point conversion out of range of unsignedbv on typecast$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^  FAILED       \[main\.assertion\.1\]\s+line 12  floating-point conversion out of range of unsignedbv on typecast$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
