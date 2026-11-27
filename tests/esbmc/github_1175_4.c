int main()
{
  const int indices[] = {100, 200, 300};
  int *ptr = (int*)indices[1];  // Use const array element as address
  int v = *ptr;  // Should fail: dereference failure
  return 0;
}
// Imported from ESBMC regression/esbmc/github_1175_4
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
