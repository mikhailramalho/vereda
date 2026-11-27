// A constant-size object allocated on one path must take no space on the
// other: here the else path's two objects take about 13 * 2^60 of the 2^64
// bytes, so they fit alone but not beside q's 3 * 2^60 bytes.
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: reach_error
#include <stdint.h>
#include <stdlib.h>

extern int __VERIFIER_nondet_int(void);
extern void reach_error(void);

int main(void) {
  if (__VERIFIER_nondet_int()) {
    char *q = malloc(3ull << 60);
    if (q && (uintptr_t)q == 12345u) // observe q's address
      return 1;
  } else {
    char *a = malloc((1ull << 63) - 1);
    char *b = malloc((1ull << 62) + (1ull << 60));
    if (a && b)
      reach_error();
  }
  return 0;
}
