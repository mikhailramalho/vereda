// Minimal form of R14: a write through a pointer to a popped frame's local.
// The local's L2 counter must survive frame teardown, so this write takes a
// fresh index instead of re-issuing the one its declaration already defined.
static int *dangle(void)
{
  int local = 42;
  return &local;
}

int main(void)
{
  int *p = dangle();
  *p = 7;
  return 0;
}
// Imported from ESBMC regression/esbmc/double_assign_check_expired_write
// Needs: --double-assign-check
// RUN: --double-assign-check --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^.*dereference failure: accessed expired variable pointer.*$
// Vereda: verdict none
