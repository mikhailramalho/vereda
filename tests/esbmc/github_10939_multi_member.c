/* A union with several full-size members over a struct of one scalar, at a
 * dynamic (but zero) offset: a write through any member reaches every byte
 * (#10939). */
#include <assert.h>
#include <stddef.h>
struct d { unsigned c; };
union u { unsigned char b[4]; unsigned w; float f; };
size_t __VERIFIER_nondet_size(void);
unsigned __VERIFIER_nondet_uint(void);
int main(void)
{
  struct d d = {__VERIFIER_nondet_uint()};
  unsigned orig = d.c;
  size_t off = __VERIFIER_nondet_size();
  __VERIFIER_assume(off == 0);
  union u *p = (union u *)((unsigned char *)&d + off);
  union u v = *p;
  assert(v.w == orig);
  assert(v.b[1] == ((orig >> 8) & 0xff));
  union u n;
  n.w = 0xa1b2c3d4u;
  *p = n;
  assert(d.c == 0xa1b2c3d4u);
  n.f = 1.0f;
  *p = n;
  assert(d.c == 0x3f800000u);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10939_multi_member
// EXPECT: ^VERIFICATION SUCCESSFUL$
