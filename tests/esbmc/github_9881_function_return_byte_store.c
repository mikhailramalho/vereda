/* #9881: a store into an untyped heap object is split into several SSA steps
 * that all carry the call's value; the call still gets one function_return
 * waypoint. */
#include <stdlib.h>
extern unsigned long __VERIFIER_nondet_ulong(void);
struct S
{
  int a;
  unsigned long x;
};
void *alloc(size_t n)
{
  void *p = malloc(n);
  if (!p)
    abort();
  return p;
}
int main(void)
{
  struct S *s = alloc(sizeof(struct S));
  s->x = __VERIFIER_nondet_ulong();
  __VERIFIER_assert(s->x != 5); // x is 5
}
// Imported from ESBMC regression/esbmc/github_9881_function_return_byte_store
// Needs: --witness-output-yaml -
// RUN: --32 --no-pointer-check --witness-output-yaml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: value: "\\\\result == 5"
// EXPECT: (?s)\A(?!.*result == 5.*result == 5)
// Vereda: verdict none
