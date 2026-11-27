/* esbmc/esbmc#10168, through the legacy goto_convertt::convert_switch. */
int main(void)
{
  int *p = (int *)0;
  switch (*p)
  {
  default:
    break;
  }
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10168_legacy
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  dereference failure: NULL pointer$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
