unsigned int global_var2 = 1;
unsigned int global_var3;

int main(void) {
  unsigned int *local_var_ptr = (unsigned int *)global_var2;
  global_var3 = *local_var_ptr;
  return 0;
}
// Imported from ESBMC regression/esbmc/github_382_1
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
