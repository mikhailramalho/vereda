struct Flags
{
  volatile unsigned int small : 3;
};

struct Flags flags;

int main(void)
{
  unsigned int first = flags.small;
  unsigned int second = flags.small;
  __VERIFIER_assert(first == second); // volatile bitfield reads may differ
}
// Imported from ESBMC regression/esbmc/volatile_bitfield_read_fail
// Needs: --volatile-check
// RUN: --std c11 --volatile-check --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
