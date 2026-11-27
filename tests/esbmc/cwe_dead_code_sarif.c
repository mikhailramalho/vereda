extern int __VERIFIER_nondet_int(void);

int main(void)
{
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assume(x > 10);
  // Dead else branch (see cwe_dead_code); this variant checks the SARIF
  // advisory (result.level == "note", CWE-561 taxon).
  if (x > 5)
    return 0;
  else
    return 1;
}
// Imported from ESBMC regression/esbmc/cwe_dead_code_sarif
// Needs: --dead-code-check --sarif-output
// RUN: --dead-code-check --sarif-output - --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: "version": "2.1.0"
// EXPECT: "ruleId": "dead-code"
// EXPECT: "level": "note"
// EXPECT: "id": "561"
// EXPECT: "name": "CWE"
// Vereda: verdict none
