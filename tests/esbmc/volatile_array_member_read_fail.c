struct Flags
{
  int values[2];
};

volatile struct Flags flags;

int main(void)
{
  int first = flags.values[1];
  int second = flags.values[1];
  __VERIFIER_assert(first == second); // volatile array member reads may differ
}
// Imported from ESBMC regression/esbmc/volatile_array_member_read_fail
// Needs: --volatile-check
// RUN: --std c11 --volatile-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
