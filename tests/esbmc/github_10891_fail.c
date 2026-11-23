/* #10891: the read is legal, so the wrong value it carries must be found. */
#include <assert.h>
#include <stdint.h>
unsigned __VERIFIER_nondet_uint(void);
struct S { uint32_t v; };
int main(void)
{
  uint32_t buf[2] = {7, 8};
  unsigned i = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i < 2);
  const struct S *p = (const struct S *)&buf[i];
  struct S s = *p;
  assert(s.v == 7);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10891_fail
// RUN: --multi-property
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: PASSED +\[main\.assertion\.[0-9]+\] +line [0-9]+ +dereference failure: Object accessed with incompatible base type
// Vereda: unmet EXPECT: PASSED +\[main\.assertion\.[0-9]+\] +line [0-9]+ +dereference failure: Object accessed with incompatible base type
// MODE: FUTURE
// Vereda: verdict FAILED
