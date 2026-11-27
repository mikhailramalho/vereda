int *num;

int main()
{
  void *data_rgn_base = malloc(*num);
  __ESBMC_init_object(data_rgn_base);
  return 0;
}
// Imported from ESBMC regression/esbmc/init_obj_vlas
// Needs: __ESBMC_init_object
// RUN: --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^ERROR: __ESBMC_init_object does not support VLAs$
