#include <assert.h>
#include <strings.h>

int main(void)
{
  assert(strcasecmp("Hello", "hELLO") == 0);
  assert(strcasecmp("abc", "ABD") > 0);
  char a[2] = {'a', 'b'};
  return strncasecmp(a, "ABC", 3);
}
// Imported from ESBMC regression/esbmc/strcasecmp_model_fail
// RUN: --multi-property
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^\*\* 2 of 6 properties failed
// EXPECT: FAILED\s+\[main.assertion.2\]
// EXPECT: FAILED\s+\[strncasecmp.*array bounds violated
// Vereda: unmet EXPECT: ^\*\* 2 of 6 properties failed
// MODE: FUTURE
// Vereda: verdict FAILED
