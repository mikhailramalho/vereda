unsigned __VERIFIER_nondet_uint(void);
int main(void) {
  unsigned i = 0;
  _Bool b = __ESBMC_forall(&i, i <= 4294967295u);
  return 0;
}
// Imported from ESBMC regression/goto2lean/unsupported_quantifier_value
// Needs: __ESBMC_forall
// Needs: --goto2lean
// RUN: --goto2lean
// MODE: FUTURE
// EXPECT: ^ERROR: goto2lean: unsupported quantifier used as a value \(only guards of ASSERT, ASSUME and GOTO\)$
