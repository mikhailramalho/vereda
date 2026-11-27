extern unsigned char __VERIFIER_nondet_uchar(void);
int main(void)
{
  unsigned char x = __VERIFIER_nondet_uchar();
  __VERIFIER_assert(x != 255); // target
}
// Imported from ESBMC regression/witnesses_validate/reachsafety-violationwitness/uchar_return_ull_positive
// Needs: --validate-violation-witness --witness witness.yml
// RUN: -m64 --std c11 --validate-violation-witness --witness witness.yml --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict none
