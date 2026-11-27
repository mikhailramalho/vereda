#include <assert.h>
#include <limits.h>

int main()
{
  _Bool c = __VERIFIER_nondet_bool();
  _Bool d = __VERIFIER_nondet_bool();
  int x = __VERIFIER_nondet_int();
  int y = __VERIFIER_nondet_int();
  int z = __VERIFIER_nondet_int();

  // (c ? (c ? x : y) : z) -> (c ? x : z)
  assert((c ? (c ? x : y) : z) == (c ? x : z));

  // (c ? x : (c ? y : z)) -> (c ? x : z)
  assert((c ? x : (c ? y : z)) == (c ? x : z));

  // (c ? x : x) -> x
  assert((c ? x : x) == x);

  const int K = 2;

  // (c ? K : K == K)
  assert((c ? K : K) == K);

  // (c ? x : (!c ? y : z)) == (c ? x : y)
  assert((c ? x : (!c ? y : z)) == (c ? x : y));

  // (!c ? x : (c ? y : z)) == (c ? y : x)
  assert((!c ? x : (c ? y : z)) == (c ? y : x));

  int w = __VERIFIER_nondet_int();
  
  // (c ? (c ? x : y) : (c ? z : w))
  assert((c ? (c ? x : y) : (c ? z : w)) == (c ? x : w));
 
  int b = __VERIFIER_nondet_int();

  // (b != 0 ? x : y) == (b ? x : y)
  assert(((b != 0) ? x : y) == (b ? x : y));

  // (!!b ? x : y) == (b ? x : y)
  assert(((!!b) ? x : y) == (b ? x : y));

  unsigned u = __VERIFIER_nondet_uint();

  // ( (int)u ? x : y ) == ( u ? x : y )
  assert(((int)u ? x : y) == (u ? x : y));

  // ( c ? (int)x : (long)x )
  assert((c ? (int)x : (long)x) == (c ? (int)x : (long)x));

  // (!(!!c) ? x : y) == (!c ? x : y)
  assert((!(!!c) ? x : y) == (!c ? x : y));

  // (c ? (c ? x : y) : (c ? y : z)) == (c ? x : z)
  assert((c ? (c ? x : y) : (c ? y : z)) == (c ? x : z));

  // (!c ? (!c ? x : y) : (!c ? y : z)) == (!c ? x : z)
  assert((!c ? (!c ? x : y) : (!c ? y : z)) == (!c ? x : z));

  _Bool e = __VERIFIER_nondet_bool();

  assert((c ? (e ? x : y) : z) == (c ? (e ? x : y) : z));
  assert((c ? x : (e ? y : z)) == (c ? x : (e ? y : z)));

  return 0;
}
// Imported from ESBMC regression/esbmc/simplifier-if
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
