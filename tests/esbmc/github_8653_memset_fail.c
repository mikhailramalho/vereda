/* #8653: with --no-pointer-check, memset must not claim a code-object
   dereference failure when the pointer may target a function. */
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
    assert(buf[0] == 1);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_8653_memset_fail
// RUN: --no-pointer-check --multi-property
// EXPECT: ^Properties: 1 checked, \d+ passed, 1 failed, \d+ not checked$
// EXPECT: ^VERIFICATION FAILED$
