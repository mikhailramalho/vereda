struct Flags
{
  unsigned int value;
};

volatile struct Flags flag;
int values[2];

int main(void)
{
  values[flag.value] = 1;
}
// Imported from ESBMC regression/esbmc/volatile_index_read_fail
// Needs: --volatile-check
// RUN: --std c11 --volatile-check --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
