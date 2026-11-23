// --clz-zero-check covers the whole __builtin_clz*/ctz* family, not just the
// three clz spellings it was originally written for. The two-argument clzg/ctzg
// name their own result at zero, so they stay unflagged. #6925.
int main(void)
{
  unsigned u = 0;
  __VERIFIER_assert(__builtin_clzg(u, 32) == 32); // clzg names its result at zero
  return __builtin_ctz(u);
}
// Imported from ESBMC regression/esbmc/builtin_ctz_zero_fail
// Needs: --clz-zero-check
// RUN: --clz-zero-check
// MODE: FUTURE
// EXPECT: ^  __builtin_clz/ctz of zero is undefined$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
