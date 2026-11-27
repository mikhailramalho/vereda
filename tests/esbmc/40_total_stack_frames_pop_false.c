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
// Imported from ESBMC regression/esbmc/40_total_stack_frames_pop_false
// Needs: --total-stack-limit 255
// RUN: --total-stack-limit 255 --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
