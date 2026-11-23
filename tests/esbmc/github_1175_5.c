struct Config {
  const int base_addr;
  const int port;
};

int main()
{
  struct Config cfg = {0x1000, 8080};
  int *ptr = (int*)cfg.base_addr;  // Use const member as address
  int v = *ptr;  // Should fail: dereference failure
  return 0;
}

// Imported from ESBMC regression/esbmc/github_1175_5
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
