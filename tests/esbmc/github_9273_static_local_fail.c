#include <stdint.h>

/* #9273 with a function-local static, whose address only a later call takes. */
uintptr_t __VERIFIER_nondet_uintptr(void);
_Bool __VERIFIER_nondet_bool(void);
int *get(void)
{
  static int s;
  return &s;
}
int main(void)
{
  int *p = (int *)__VERIFIER_nondet_uintptr();
  int *q = 0;
  if (__VERIFIER_nondet_bool())
    q = get();
  __VERIFIER_assert(p != q || q == 0); // p may point to s
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9273_static_local_fail
// RUN:
// EXPECT: ^VERIFICATION FAILED$
