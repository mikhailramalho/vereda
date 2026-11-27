// The 32-bit form of mem_alloc_one_path_layout_fail at realistic sizes:
// 0.8 GB on one path, 2 GB and 1.35 GB on the other, which fit in 4 GB
// only while the first path's object takes no space there.
// RUN: -m32
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: reach_error
#include <stdint.h>
#include <stdlib.h>

extern int __VERIFIER_nondet_int(void);
extern void reach_error(void);

int main(void) {
  if (__VERIFIER_nondet_int()) {
    char *q = malloc(800000000u);
    if (q && (uintptr_t)q == 12345u) // observe q's address
      return 1;
  } else {
    char *a = malloc(2147483647u);
    char *b = malloc(1350000000u);
    if (a && b)
      reach_error();
  }
  return 0;
}
