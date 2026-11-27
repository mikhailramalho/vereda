extern int __VERIFIER_nondet_int(void);

int main(void)
{
  int x = __VERIFIER_nondet_int();
  // Both branches are reachable, so there is no dead code to report.
  if (x > 5)
    return 0;
  else
    return 1;
}
// Imported from ESBMC regression/esbmc/cwe_dead_code_pass
// Needs: --dead-code-check
// RUN: --dead-code-check
// MODE: FUTURE
// EXPECT: ^\[Dead code\]$
// EXPECT: ^No provably-dead code found\.$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
