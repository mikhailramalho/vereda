extern int __VERIFIER_nondet_int(void);

// No dead code, but --sarif-output is requested: consumers expect a valid
// SARIF run with an empty results array, not a missing document.
int main(void)
{
  int x = __VERIFIER_nondet_int();
  if (x > 5)
    return 0;
  else
    return 1;
}
// Imported from ESBMC regression/esbmc/cwe_dead_code_pass_sarif
// Needs: --dead-code-check --sarif-output
// RUN: --dead-code-check --sarif-output - --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: "version": "2.1.0"
// EXPECT: "results": \[\]
// Vereda: verdict none
