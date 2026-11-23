#include <string.h>

int main()
{
  const char *str = "Hello";
  const size_t len = strlen(str);  // len = 5
  int *ptr = (int*)len;  // Use const length as address
  int v = *ptr;  // Should fail: dereference failure
  return 0;
}
// Imported from ESBMC regression/esbmc/github_1175_11
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
