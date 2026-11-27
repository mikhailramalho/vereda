int f(void) { return 0; }
int main()
{
  __ESBMC_init_object((void *)f);
  return 0;
}
// Imported from ESBMC regression/esbmc/function_write_init_object_fail
// Needs: __ESBMC_init_object
// RUN: --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^  dereference failure: Program code accessed in write or free mode$
// EXPECT: ^VERIFICATION FAILED$
