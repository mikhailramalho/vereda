/* The companion to github_7267-const-guard: suppressing constant-folded
   guards must not stop a genuinely unreachable branch being reported. */
int f(void)
{
  return 1;
}

int main(void)
{
  int x = 5;
  if (x > 10)
    f();
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7267-genuine-dead-code
// Needs: --dead-code-check
// RUN: --dead-code-check --no-incremental-bmc
// MODE: FUTURE
// EXPECT: github_7267\-genuine\-dead\-code\.c:11: dead code: unreachable branch \[guard: x > 10\]$
// EXPECT: CWE-561
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
