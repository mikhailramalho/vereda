volatile int values[2];

int main(void)
{
  volatile int *pointer = &values[0];
  int first = *(pointer + 1);
  int second = *(pointer - (-1));
  __VERIFIER_assert(first == second); // volatile pointer arithmetic reads may differ
}
// Imported from ESBMC regression/esbmc/volatile_pointer_arithmetic_read_fail
// Needs: --volatile-check
// RUN: --std c11 --volatile-check --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
