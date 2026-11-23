//test_abort.c:
#include "test_abort.h"

void func1(int y);
void func1(int y)
{
  func2(y);
}
// Imported from ESBMC regression/esbmc/github_885
// RUN: --entry func1
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: \sarray (upper|lower) bound$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
