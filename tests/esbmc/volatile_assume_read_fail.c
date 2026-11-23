struct Flags
{
  int value;
};

volatile struct Flags flag;

int main(void)
{
  __VERIFIER_assume(flag.value != 0);
  __VERIFIER_assert(0); // a volatile assumption can keep this path reachable
}
// Imported from ESBMC regression/esbmc/volatile_assume_read_fail
// Needs: --std c11 --volatile-check
// RUN: --std c11 --volatile-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
