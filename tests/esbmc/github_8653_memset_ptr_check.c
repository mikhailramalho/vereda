/* #8653: without --no-pointer-check, memset into a pointer that may target a
   function is still a code-object dereference failure. */
#include <assert.h>
#include <string.h>

void f(void)
{
}

int __VERIFIER_nondet_int(void);

int main(void)
{
  char buf[4] = {1, 2, 3, 4};
  void *p = __VERIFIER_nondet_int() ? (void *)f : (void *)buf;
  memset(p, 0, sizeof buf);
  if (p == (void *)buf)
    assert(buf[0] == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_8653_memset_ptr_check
// RUN: --no-incremental-bmc
// EXPECT: FAILED +\[memset\.assertion\.1\].*trying to deref a ptr code
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: FAILED +\[memset\.assertion\.1\].*trying to deref a ptr code
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
