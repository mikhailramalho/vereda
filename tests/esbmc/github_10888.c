/* #10888: a struct read through a same-layout struct whose members have
 * other names or types is rebuilt from the object's bytes. */
#include <assert.h>
struct T
{
  unsigned char a, b, c, d;
};
struct S
{
  unsigned short x;
  unsigned char y, z;
};
struct C
{
  unsigned char f1, f2;
};
unsigned char __VERIFIER_nondet_uchar(void);
int main(void)
{
  struct T s = {__VERIFIER_nondet_uchar(), __VERIFIER_nondet_uchar(), 3, 4};
  struct C c = *(struct C *)&s;
  assert(c.f1 == s.a && c.f2 == s.b);
  struct C d = *(struct C *)((char *)&s + 1);
  assert(d.f1 == s.b && d.f2 == 3);
  struct S v = *(struct S *)&s;
  assert(v.x == (unsigned short)(s.a | s.b << 8) && v.y == 3 && v.z == 4);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10888
// Needs: --little-endian
// RUN: --little-endian --multi-property
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
