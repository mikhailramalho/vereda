volatile int value;

int main(void)
{
  volatile int *pointer = &value;
  int first = *pointer;
  int second = *pointer;
  __VERIFIER_assert(first == second); // volatile reads may differ
}
// Imported from ESBMC regression/esbmc/volatile_pointer_read_fail_kinduction
// Needs: --std c11 --volatile-check --k-induction
// RUN: --std c11 --volatile-check --k-induction
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
