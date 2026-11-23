// #9352: a store to a _Bool bit-field converts the value to _Bool
// (C11 6.3.1.2p1, 6.7.2.1p10) instead of keeping its low bit.
#include <assert.h>

extern int __VERIFIER_nondet_int(void);

struct S
{
  _Bool b : 1;
} s;

int main(void)
{
  int v = __VERIFIER_nondet_int();
  __VERIFIER_assume(v != 0);

  s.b = 0;
  s.b |= 2;
  assert(s.b == 1);

  s.b = 0;
  s.b += v;
  assert(s.b == 1);

  s.b = 1;
  s.b++;
  assert(s.b == 1);

  s.b = 1;
  ++s.b;
  assert(s.b == 1);

  s.b = 0;
  s.b--;
  assert(s.b == 1);

  s.b = 1;
  s.b *= 2;
  assert(s.b == 1);

  s.b = 1;
  s.b &= 2;
  assert(s.b == 0);
  return 0;
}
// Imported from ESBMC regression/cstd/github_9352
// Needs: --std c11
// RUN: --std c11
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
