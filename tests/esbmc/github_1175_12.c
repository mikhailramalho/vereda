const int GLOBAL_CONST = 0x404040;

int main()
{
  int *ptr = (int*)GLOBAL_CONST;  // Use global const as address
  int v = *ptr;  // Should fail: dereference failure
  return 0;
}
// Imported from ESBMC regression/esbmc/github_1175_12
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
