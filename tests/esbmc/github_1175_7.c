enum MemoryAddresses {
  INVALID_ADDR = 0x12345678
};

int main()
{
  const enum MemoryAddresses addr = INVALID_ADDR;
  int *ptr = (int*)addr;  // Use const enum as address
  int v = *ptr;  // Should fail: dereference failure
  return 0;
}
// Imported from ESBMC regression/esbmc/github_1175_7
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
