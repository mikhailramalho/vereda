struct NvBool
{
  int value;
};

volatile struct NvBool flag;

int main(void)
{
  int first = flag.value;
  int second = flag.value;
  __VERIFIER_assert(first == second); // volatile struct member reads may differ
}
// Imported from ESBMC regression/esbmc/volatile_struct_read_fail
// Needs: --volatile-check
// RUN: --std c11 --volatile-check --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
