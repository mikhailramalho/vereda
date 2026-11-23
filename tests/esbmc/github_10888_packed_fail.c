/* #10888: a packed struct read over a struct of shorts, whose middle member
 * straddles two source members, is rebuilt from the object's bytes. */
#include <assert.h>
struct S
{
  unsigned short x, y;
};
struct __attribute__((packed)) P
{
  unsigned char p;
  unsigned short q;
  unsigned char r;
};
int main(void)
{
  struct S s = {0x0201, 0x0403};
  struct P v = *(struct P *)&s;
  assert(v.p == 1 && v.q == 0x0201 && v.r == 4);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10888_packed_fail
// Needs: --little-endian
// RUN: --little-endian --multi-property
// MODE: FUTURE
// EXPECT: ^\*\* 1 of [0-9]+ properties failed
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
