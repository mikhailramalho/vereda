#include <stdint.h>

/* The claim is a predicate over the address, not a verdict handed down from the
 * type: a program that constrains its own packed object's base discharges it. */

struct __attribute__((packed)) S
{
  char a;
  char pad[7];
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
// Imported from ESBMC regression/esbmc/github_7707-assume
// EXPECT: ^\s+PASSED\s+\[main\.assertion\.1\].*dereference failure: Incorrect alignment when accessing data object$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^\s+PASSED\s+\[main\.assertion\.1\].*dereference failure: Incorrect alignment when accessing data object$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
