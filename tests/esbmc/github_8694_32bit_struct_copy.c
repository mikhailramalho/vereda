#include <assert.h>
#include <string.h>
struct G { char c; long long ll; };
int main(void)
{
  struct G a, b;
  a.c = 1;
  a.ll = 0x0102030405060708LL;
  memcpy(&b, &a, sizeof a);
  assert(b.ll == 0x0102030405060708LL);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_8694_32bit_struct_copy
// RUN: -m32 --multi-property
// EXPECT: ^VERIFICATION SUCCESSFUL$
