#include <stdint.h>

/* As github_7707-assume, with b at an odd offset: an aligned address is not an
 * aligned offset when the packed object's base is free (#7707). */

struct __attribute__((packed)) S
{
  char a;
  uint64_t b;
};

int main(void)
{
  struct S sp;
  uint64_t *p = (uint64_t *)&sp.b;
  __VERIFIER_assume(((uintptr_t)p & 7u) == 0);
  uint64_t z = *p;
  (void)z;
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7707-offset-aligned
// RUN: --no-incremental-bmc
// EXPECT: ^\s+PASSED\s+\[main\.assertion\.1\].*dereference failure: Incorrect alignment when accessing data object$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^\s+PASSED\s+\[main\.assertion\.1\].*dereference failure: Incorrect alignment when accessing data object$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
