unsigned __VERIFIER_nondet_uint(void);
int main(void) {
  unsigned c = __VERIFIER_nondet_uint();
  unsigned result = c, post = c, ret = c;
  __VERIFIER_assert(result == post && post == ret); // same
  return 0;
}
// Imported from ESBMC regression/goto2lean/reserved_names
// Needs: --goto2lean
// RUN: --goto2lean
// MODE: FUTURE
// EXPECT: ^  let result_1 := c;$
// EXPECT: ^  let post_1 := c;$
// EXPECT: ^  let ret_1 := c;$
// EXPECT: ^  \(\(result_1 = post_1\) ∧ \(post_1 = ret_1\)\) ∧ \($
// Vereda: verdict none
