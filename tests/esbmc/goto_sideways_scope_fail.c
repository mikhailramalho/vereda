int main()
{
  int *p;
  {
    int a;
    p = &a;
    goto L;
  }
  {
    int b;
  L:
    // a's block was left, so a is dead
    *p = 1;
  }
  return 0;
}
// Imported from ESBMC regression/esbmc/goto_sideways_scope_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: expired variable
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
