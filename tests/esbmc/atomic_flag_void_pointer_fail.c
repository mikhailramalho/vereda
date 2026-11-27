/* The first test-and-set through a void pointer returns the old value 0. */
unsigned char flag;

int main()
{
  void *p = &flag;
  flag = 0;
  __VERIFIER_assert(
    __atomic_test_and_set(p, __ATOMIC_SEQ_CST) == 1); // set returns old 1
  return 0;
}
// Imported from ESBMC regression/esbmc/atomic_flag_void_pointer_fail
// EXPECT: ^VERIFICATION FAILED$
