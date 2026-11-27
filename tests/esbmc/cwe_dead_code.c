extern int __VERIFIER_nondet_int(void);

int main(void)
{
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assume(x > 10);
  // The else branch is unreachable: x > 10 implies x > 5. A compiler's
  // -Wunreachable-code cannot see this, but BMC proves it under the guard.
  if (x > 5)
    return 0;
  else
    return 1;
}
// Imported from ESBMC regression/esbmc/cwe_dead_code
// Needs: --dead-code-check
// RUN: --dead-code-check --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^\[Dead code\]$
// EXPECT: :9: dead code: unreachable branch
// EXPECT: ^  CWE: CWE-561$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
