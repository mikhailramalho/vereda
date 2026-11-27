int main()
{
  const char c = 'A';  // ASCII value 65
  int *ptr = (int*)(unsigned long)c;  // Use const char as address
  int v = *ptr;  // Should fail: dereference failure
  return 0;
}
// Imported from ESBMC regression/esbmc/github_1175_9
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
