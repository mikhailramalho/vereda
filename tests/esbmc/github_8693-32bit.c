/* #8693: the i386 ABI aligns double, and so _Complex double, to 4 in a struct;
 * sizeof is 20, and a write one past it must be flagged. */
#include <assert.h>
#include <string.h>
struct C
{
  char c;
  _Complex double z;
};
int main(void)
{
  struct C a, b;
  a.c = 1;
  a.z = 2.0;
  memcpy(&b, &a, sizeof a);
  assert(__real__ b.z == 2.0);
  assert((char *)&a.z - (char *)&a == 4);
  char *p = (char *)&a;
  p[sizeof a] = 0;
  return 0;
}
// Imported from ESBMC regression/esbmc/github_8693-32bit
// RUN: -m32 --multi-property --no-incremental-bmc
// EXPECT: ^  PASSED +\[main\.assertion\.1\]
// EXPECT: ^  PASSED +\[main\.assertion\.2\]
// EXPECT: ^  FAILED +\[main\.object-out-of-bounds\.1\]
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^  PASSED +\[main\.assertion\.1\]
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
