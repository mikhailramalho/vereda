unsigned __VERIFIER_nondet_uint(void);
int main(void)
{
  unsigned x = __VERIFIER_nondet_uint();
  unsigned long s = sizeof((x < 10) ? 1 : 0);
  unsigned long b = sizeof(_BitInt(17));
  __VERIFIER_assert(s == 4 && b == 4); // clang sizes
  return 0;
}
// Imported from ESBMC regression/goto2lean/sizeof_expression
// Needs: --goto2lean
// RUN: --goto2lean --std c23
// MODE: FUTURE
// EXPECT: ^  let s := \(4#64\);$
// EXPECT: ^  let b := \(4#64\);$
// Vereda: verdict none
