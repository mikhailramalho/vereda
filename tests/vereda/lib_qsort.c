// Test that qsort sorts an array of 3 integers correctly.
// RUN: --unwind 8
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <stdlib.h>
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);

static int cmp(const void *a, const void *b) {
    int x = *(const int *)a, y = *(const int *)b;
    // Prevent clang from optimizing this to llvm.scmp.i32.i32
    // Use explicit conditional to avoid optimization issues
    if (x < y) return -1;
    if (x > y) return 1;
    return 0;
}

int main(void) {
  int x = __VERIFIER_nondet_int(), y = __VERIFIER_nondet_int(), z = __VERIFIER_nondet_int();
  __VERIFIER_assume(x >= -100 && x <= 100 && y >= -100 && y <= 100 && z >= -100 && z <= 100);
  int a[3] = {x, y, z};
  qsort(a, 3, sizeof a[0], cmp);

  __VERIFIER_assert(a[0] <= a[1]);
  __VERIFIER_assert(a[1] <= a[2]);
  __VERIFIER_assert(a[0] + a[1] + a[2] == x + y + z);

  int lo = x < y ? (x < z ? x : z) : (y < z ? y : z),
      hi = x > y ? (x > z ? x : z) : (y > z ? y : z);
  __VERIFIER_assert(a[0] == lo);
  __VERIFIER_assert(a[2] == hi);

  return 0;
}