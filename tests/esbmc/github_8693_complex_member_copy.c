#include <assert.h>
#include <string.h>
struct C { char c; _Complex double z; };
int main(void)
{
  struct C a, b;
  a.c = 1;
  a.z = 2.0;
  memcpy(&b, &a, sizeof a);
  assert(__real__ b.z == 2.0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_8693_complex_member_copy
// RUN: --multi-property --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
