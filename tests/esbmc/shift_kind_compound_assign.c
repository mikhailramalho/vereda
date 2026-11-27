#include <assert.h>
typedef unsigned int u32;
unsigned int __VERIFIER_nondet_uint(void);
int __VERIFIER_nondet_int(void);

int main(void)
{
  u32 a = __VERIFIER_nondet_uint();
  __VERIFIER_assume(a >= 0x80000000u);
  a >>= 1;
  /* logical shift: a <= 0x7FFFFFFF.  arithmetic would give >= 0xC0000000 */
  assert(a < 0x80000000u);

  unsigned int b = __VERIFIER_nondet_uint();
  __VERIFIER_assume(b >= 0x80000000u);
  b >>= 1;
  assert(b < 0x80000000u);

  int s = __VERIFIER_nondet_int();
  __VERIFIER_assume(s < 0);
  s >>= 1;
  /* arithmetic shift keeps the sign; a logical one would make it non-negative */
  assert(s < 0);

  return 0;
}
// Imported from ESBMC regression/esbmc/shift_kind_compound_assign
// RUN: --no-unwinding-assertions
// EXPECT: ^VERIFICATION SUCCESSFUL$
