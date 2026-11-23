#include <assert.h>
#include <stdio.h>

unsigned __VERIFIER_nondet_uint(void);

int main(void)
{
  unsigned x = __VERIFIER_nondet_uint();

  char b[12] = "abcdefghijk";
  int r = sprintf(b, "%x", x);
  assert(r >= 1 && r <= 8);
  assert(b[r] == '\0');
  assert(b[11] == '\0');
  assert(r != 1 || b[2] == 'c');

  char c[8] = "abcdefg";
  int m = snprintf(c, 3, "%x", x);
  assert(m >= 1 && m <= 8);
  assert(c[1] == '\0' || c[2] == '\0');
  assert(c[3] == 'd');
  return 0;
}
// Imported from ESBMC regression/esbmc/sprintf_symbolic_output
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict FAILED
