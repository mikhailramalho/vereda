//test_abort.c:
#include "test_abort.h"

void func1(int y);
void func1(int y)
{
  __VERIFIER_assume(y >= 0);
  __VERIFIER_assume(y < 1);
  func2(y);
}
// Imported from ESBMC regression/esbmc/github_885-valid
// RUN: --entry func1
// EXPECT: ^VERIFICATION SUCCESSFUL$
