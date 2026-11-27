struct Flags
{
  int value;
};

volatile struct Flags flag;

int main(void)
{
  __VERIFIER_assert(flag.value == 0); // a volatile assertion reads a fresh value
}
// Imported from ESBMC regression/esbmc/volatile_assert_read_fail
// Needs: --volatile-check
// RUN: --std c11 --volatile-check --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
