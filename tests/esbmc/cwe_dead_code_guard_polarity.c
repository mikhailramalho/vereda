extern int __VERIFIER_nondet_int(void);

// Pins both polarities of the guard named in a dead-code advisory. `flag` also
// pins expression-level negation: the surviving probe's comment is "!flag", so
// printing it gives "!flag" and wrapping it in !(...) gives "!(!flag)".
int main(void)
{
  _Bool flag = 0;
  int x = __VERIFIER_nondet_int();
  if (flag)
    return 1;
  __VERIFIER_assume(x > 10);
  if (x > 5)
    return 0;
  return 2;
}
// Imported from ESBMC regression/esbmc/cwe_dead_code_guard_polarity
// Needs: --dead-code-check --sarif-output out.sarif CHECK_JSON out.sarif $.runs[0].results[0].message.text == "dead code: unreachable branch [guard: flag]" CHECK_JSON out.sarif $.runs[0].results[1].message.text == "dead code: unreachable branch [guard: !(x > 5)]"
// RUN: --dead-code-check --sarif-output out.sarif --no-incremental-bmc
// MODE: FUTURE
// EXPECT: cwe_dead_code_guard_polarity\.c:10: dead code: unreachable branch \[guard: flag\]$
// EXPECT: cwe_dead_code_guard_polarity\.c:13: dead code: unreachable branch \[guard: !\(x > 5\)\]$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
