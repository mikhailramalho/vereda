// Which arrays of their own hold elements (#107): those read and written
// only as one iN, N = 16, 32 or 64, at multiples of N/8 (objects 1 to 3),
// and not one read as chars too, one passed to memset, one read at an odd
// offset or one read as shorts too (objects 4 to 7).
// RUN: --dump-smt-only
// EXPECT: ^\(declare-const \|object#1#\d+\| \(Array \(_ BitVec 62\) \(_ BitVec 32\)\)\)$
// EXPECT: ^\(declare-const \|object#2#\d+\| \(Array \(_ BitVec 63\) \(_ BitVec 16\)\)\)$
// EXPECT: ^\(declare-const \|object#3#\d+\| \(Array \(_ BitVec 61\) \(_ BitVec 64\)\)\)$
// EXPECT: ^\(declare-const \|object#4#\d+\| \(Array \(_ BitVec 64\) \(_ BitVec 8\)\)\)$
// EXPECT: ^\(declare-const \|object#5#\d+\| \(Array \(_ BitVec 64\) \(_ BitVec 8\)\)\)$
// EXPECT: ^\(declare-const \|object#6#\d+\| \(Array \(_ BitVec 64\) \(_ BitVec 8\)\)\)$
// EXPECT: ^\(declare-const \|object#7#\d+\| \(Array \(_ BitVec 64\) \(_ BitVec 8\)\)\)$
#include <string.h>
extern void __VERIFIER_assert(int);
extern unsigned __VERIFIER_nondet_uint(void);

int main(void) {
  unsigned i = __VERIFIER_nondet_uint() % 4;
  int a[4];
  short b[4];
  long c[4];
  int d[4], e[4], f[4], g[4];
  memset(e, 0, 8);
  __VERIFIER_assert(a[i] + b[i] + c[i] + ((char *)d)[i] + e[i] +
                        *(int *)((char *)f + 2 * i) + ((short *)g)[i] + g[i] != 0);
  return 0;
}
