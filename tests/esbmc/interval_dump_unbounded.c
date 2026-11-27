int __VERIFIER_nondet_int(void);

int main(void)
{
  int i = __VERIFIER_nondet_int();
  int k = __VERIFIER_nondet_int();
  int m = __VERIFIER_nondet_int();
  __VERIFIER_assume(i >= 200000);
  __VERIFIER_assume(k >= 300000);
  __VERIFIER_assume(m <= -400000);
  __VERIFIER_assert(i >= 0 && k >= 0 && m <= 0); // each bound prints on its own line
  return 0;
}
// Imported from ESBMC regression/esbmc/interval_dump_unbounded
// Needs: --interval-analysis --interval-analysis-dump
// RUN: --interval-analysis --interval-analysis-dump --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: ^200000 <= c:.*@main@i$
// EXPECT: ^300000 <= c:.*@main@k$
// EXPECT: ^c:.*@main@m <= -400000$
// Vereda: verdict none
