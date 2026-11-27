#define CONST_ADDR 0xBADC0DE

int main()
{
  int *ptr = (int*)CONST_ADDR;  // Use const macro as address
  int v = *ptr;  // Should fail: dereference failure
  return 0;
}
// Imported from ESBMC regression/esbmc/github_1175_8
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
