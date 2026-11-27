// Check if equality between heap and stack is correct
int main(void) {
  int* p = (int*) malloc(sizeof(int)); // Null or unique heap addr
  int q; // unique stack addr

  if(p != &q) __VERIFIER_assert(0); // p should be different to &q
  return 0;
}
// Imported from ESBMC regression/esbmc/no_pointer_check_2_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
