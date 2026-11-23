volatile int value;

int main(void)
{
  int first = *(&value);
  int second = *(&value);
  __VERIFIER_assert(first == second); // volatile address reads may differ
}
// Imported from ESBMC regression/esbmc/volatile_address_read_fail
// Needs: --std c11 --volatile-check
// RUN: --std c11 --volatile-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
