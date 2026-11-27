int main()
{
  int r = 0;
  for (int i = 0; i < 2; i++)
  {
    int x = i;
    int *p = &x;
    r += *p;
  }
  __VERIFIER_assert(r == 1); // each pointer reads its own iteration's x
  return r;
}
// Imported from ESBMC regression/esbmc/loop_local_l1_reuse_address
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
