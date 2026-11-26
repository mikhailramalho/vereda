typedef struct
{
  int value;
} NvBool;

NvBool flag;

int main(void)
{
  volatile NvBool *pointer = &flag;
  int first = pointer->value;
  int second = pointer->value;
  __VERIFIER_assert(first == second); // volatile pointer member reads may differ
}
// Imported from ESBMC regression/esbmc/volatile_qualified_struct_read_fail
// Needs: --volatile-check
// RUN: --std c11 --volatile-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
