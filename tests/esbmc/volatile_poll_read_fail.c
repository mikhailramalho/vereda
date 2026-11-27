typedef struct
{
  int value;
} NvBool;

NvBool flag;

int main(void)
{
  volatile NvBool *pointer = &flag;
  if (pointer->value != 0)
    __VERIFIER_assert(0); // a volatile poll can observe a change
}
// Imported from ESBMC regression/esbmc/volatile_poll_read_fail
// Needs: --volatile-check
// RUN: --std c11 --volatile-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
