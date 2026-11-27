// Check if comparation is nondet
int main(void) {
  int p; // unique stack addr
  int q; // unique stack addr;

  if(p > q) __VERIFIER_assert(0); // p shouldn't be greater than q
  return 0;
}
// Imported from ESBMC regression/esbmc/no_pointer_check_4_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
