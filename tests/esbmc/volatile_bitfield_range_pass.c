struct Flags
{
  volatile unsigned int small : 3;
  volatile signed int signed_small : 3;
};

struct Flags flags;

int main(void)
{
  unsigned int small = flags.small;
  int signed_small = flags.signed_small;
  __VERIFIER_assert(small <= 7); // an unsigned 3-bit read stays in range
  __VERIFIER_assert(
    signed_small >= -4 && signed_small <= 3
); // a signed 3-bit read stays in range
}
// Imported from ESBMC regression/esbmc/volatile_bitfield_range_pass
// Needs: --volatile-check
// RUN: --std c11 --volatile-check --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
