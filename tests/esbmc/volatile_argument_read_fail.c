struct Flags
{
  volatile int value;
};

struct Flags flag;

void check(int value)
{
  __VERIFIER_assert(value == 0); // a volatile argument reads a fresh value
}

int main(void)
{
  check(flag.value);
}
// Imported from ESBMC regression/esbmc/volatile_argument_read_fail
// Needs: --volatile-check
// RUN: --std c11 --volatile-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
