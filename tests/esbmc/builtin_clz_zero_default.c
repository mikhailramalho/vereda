// Without --clz-zero-check, __builtin_clz(0) is not flagged as UB: clz is a
// pure value (width - popcount of the right-smeared argument), so clz(0) is the
// bit width. See #4606.
int main()
{
  __VERIFIER_assert(__builtin_clz(0u) == 32); // clz(0) defaults to the bit width
  return 0;
}
// Imported from ESBMC regression/esbmc/builtin_clz_zero_default
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict FAILED
