// RUN: -m32
// EXPECT: ^VERIFICATION SUCCESSFUL$
// calloc's element count times its size, 2^33 bytes, overflows a 32-bit
// size_t, so the call returns NULL and there is nothing to zero; the zero
// fill used to run for the full product anyway (#59).
#include <stdlib.h>
extern void __VERIFIER_assert(int);
int main(void) {
  char *p = calloc(1u << 20, 1u << 13);
  __VERIFIER_assert(p == 0);
  return 0;
}
