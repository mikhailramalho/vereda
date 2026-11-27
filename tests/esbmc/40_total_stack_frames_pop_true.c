int sink;

void g(void)
{
  int a[8];
  a[0] = 1;
  sink += a[0];
}

int main(void)
{
  g();
  g();
  g();
  g();
  return 0;
}
// Imported from ESBMC regression/esbmc/40_total_stack_frames_pop_true
// Needs: --total-stack-limit 256
// RUN: --total-stack-limit 256 --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
