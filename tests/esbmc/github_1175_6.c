const int getAddress()
{
  return 0xDEADBEEF;
}

int main()
{
  int *ptr = (int*)getAddress();  // Use const function return as address
  int v = *ptr;  // Should fail: dereference failure
  return 0;
}
// Imported from ESBMC regression/esbmc/github_1175_6
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
