typedef struct
{
  unsigned int value;
} NvBool;

volatile NvBool flag;

int main(void)
{
  volatile NvBool *pointer = &flag;
  unsigned int first = pointer->value;
  unsigned int second = pointer->value;
  __VERIFIER_assert(first == second); // volatile struct pointer reads may differ
}
// Imported from ESBMC regression/esbmc/volatile_struct_pointer_read_fail_kinduction
// Needs: --volatile-check --k-induction
// RUN: --std c11 --volatile-check --k-induction
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
