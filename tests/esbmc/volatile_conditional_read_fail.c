volatile int left;
volatile int right;
int __VERIFIER_nondet_int(void);

int main(void)
{
  int selector = __VERIFIER_nondet_int();
  int first = *(selector ? &left : &right);
  int second = *(selector ? &left : &right);
  __VERIFIER_assert(first == second); // volatile conditional reads may differ
}
// Imported from ESBMC regression/esbmc/volatile_conditional_read_fail
// Needs: --std c11 --volatile-check
// RUN: --std c11 --volatile-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
