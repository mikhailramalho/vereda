// Control for mem_alloc_one_path_layout32_fail: two 2 GB objects do not fit
// in 4 GB on their own, so the else path still cannot reach the error.
// RUN: -m32
// EXPECT: ^VERIFICATION SUCCESSFUL$
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
    char *b = malloc(2147483647u);
    if (a && b)
      reach_error();
  }
  return 0;
}
