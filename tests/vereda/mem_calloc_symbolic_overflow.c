// EXPECT: ^VERIFICATION SUCCESSFUL$
// calloc fails when its nondet element count times its size overflows
// size_t (C23 7.24.3.2); an object of any other size reads back as zero.
#include <stdint.h>
#include <stdlib.h>
extern unsigned long __VERIFIER_nondet_ulong(void);
extern void __VERIFIER_assert(int);
int main(void) {
  unsigned long n = __VERIFIER_nondet_ulong();
  char *p = calloc(n, 16);
  if (n > SIZE_MAX / 16)
    __VERIFIER_assert(p == 0);
  else if (p && n)
    __VERIFIER_assert(p[n * 16 - 1] == 0);
  return 0;
}
