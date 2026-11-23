// #7905: a struct bit-cast to a vector is refused rather than encoded: laying
// the struct out as one scalar would reverse its members on a big-endian
// target, which would prove v[0] == 4 here.
#include <assert.h>

typedef int v4i __attribute__((__vector_size__(16)));

struct S
{
  int a, b, c, d;
};

int main(void)
{
  struct S s = {1, 2, 3, 4};
  v4i v = __builtin_bit_cast(v4i, s);
  assert(v[0] == 4);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7905_struct_refused
// Needs: --big-endian
// RUN: --big-endian
// MODE: FUTURE
// EXPECT: ^ERROR: Typecast for unexpected type$
// Vereda: verdict none
