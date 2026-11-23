/* Soundness boundary of is_provably_nonneg: each case below is
 * falsifiable and its own FAILED line is pinned, so a relaxation of
 * any guard erases that line and trips the test.
 *   1. plain signed nondet — the base case;
 *   2. SAME-width unsigned-to-signed cast: (int)u32 reinterprets the
 *      bits, the sign bit can be set (relaxing the strict `>` width
 *      check to `>=` wrongly folds this);
 *   3. NARROWING unsigned-to-signed cast: (short)u32 truncates, the
 *      kept bits can set the sign;
 *   4. widening SIGNED-to-signed cast: sign-extension preserves a
 *      negative (relaxing the unsigned-source check wrongly folds
 *      this). */
int __VERIFIER_nondet_int(void);
unsigned int __VERIFIER_nondet_u4(void);
unsigned short __VERIFIER_nondet_u2(void);
short __VERIFIER_nondet_s2(void);

int main(void)
{
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assert(x >= 0); // a signed nondet can be negative

  int same_width = (int)__VERIFIER_nondet_u4();
  __VERIFIER_assert(same_width >= 0); // same-width cast keeps the sign bit

  short narrowed = (short)__VERIFIER_nondet_u4();
  __VERIFIER_assert(narrowed >= 0); // narrowing keeps the sign bit

  int sign_extended = (int)__VERIFIER_nondet_s2();
  __VERIFIER_assert(sign_extended >= 0); // sign-extension keeps a negative
  return 0;
}
// Imported from ESBMC regression/esbmc/promoted_unsigned_nonneg_fail
// RUN: --unwind 2 --multi-property
// EXPECT: ^VERIFICATION FAILED$
