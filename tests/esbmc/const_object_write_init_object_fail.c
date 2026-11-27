const int t[2] = {1, 2};
int main()
{
  __ESBMC_init_object((void *)t);
  return 0;
}
// Imported from ESBMC regression/esbmc/const_object_write_init_object_fail
// Needs: __ESBMC_init_object
// RUN: --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^  dereference failure: write access to const object `t'$
// EXPECT: ^VERIFICATION FAILED$
