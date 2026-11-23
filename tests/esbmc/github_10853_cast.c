extern unsigned char __VERIFIER_nondet_uchar(void);
int main(void)
{
  unsigned char c = __VERIFIER_nondet_uchar();
  int i = 0;
  __ESBMC_loop_invariant((int)c >= 0 && i <= 3 && c + 1 > 0);
  while (i < 3)
    i++;
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10853_cast
// Needs: --loop-invariant-check --witness-output-yaml -
// RUN: --loop-invariant-check --witness-output-yaml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: value: "\(signed int\)c >= 0 && i <= 3 && \(signed int\)c \+ 1 > 0"$
// Vereda: verdict none
