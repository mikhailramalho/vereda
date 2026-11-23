#include <string.h>

int main()
{
  char b[3] = {'a', 'b', 'c'};
  strndup(b, 5);
  return 0;
}
// Imported from ESBMC regression/esbmc/strndup_model_fail
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: dereference failure: array bounds violated
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
