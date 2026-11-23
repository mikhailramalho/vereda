extern int __VERIFIER_nondet_int(void);

// Companion to cwe_dead_code: the SAME program with a provably-dead else, but
// run WITHOUT --dead-code-check. The dead-code machinery must be completely
// inert when the flag is off, so ordinary verification runs unchanged and
// finds the reachable NULL dereference in the (live) then branch.
int main(void)
{
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assume(x > 10);
  int *p = 0;
  if (x > 5)
    return *p; // reachable NULL dereference (x > 10 implies x > 5)
  else
    return 1; // dead: never taken
}
// Imported from ESBMC regression/esbmc/cwe_dead_code_flagoff
// RUN: --incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  dereference failure: NULL pointer$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
