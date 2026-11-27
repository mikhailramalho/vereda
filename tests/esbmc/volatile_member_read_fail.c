struct Flags
{
  volatile int changing;
  int stable;
};

struct Flags flags;

int main(void)
{
  int first = flags.changing;
  int second = flags.changing;
  __VERIFIER_assert(first == second); // volatile field reads may differ
}
// Imported from ESBMC regression/esbmc/volatile_member_read_fail
// Needs: --volatile-check
// RUN: --std c11 --volatile-check --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
