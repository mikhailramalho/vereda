/* #10888: the struct rebuilt from the object's bytes keeps their order. */
#include <assert.h>
struct T
{
  unsigned char a, b;
};
struct C
{
  unsigned char f1, f2;
};
unsigned char __VERIFIER_nondet_uchar(void);
int main(void)
{
  struct T s = {__VERIFIER_nondet_uchar(), __VERIFIER_nondet_uchar()};
  struct C c = *(struct C *)&s;
  assert(c.f1 == s.b);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10888_fail
// RUN: --multi-property --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
