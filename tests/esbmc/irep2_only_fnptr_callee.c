#include<assert.h>

int A() { return 1; }

struct X {
  int a;
  int (*update) ();
};

int main()
{
  struct X x;
  x.update = A;

  int one = x.update();
  assert(one == 1);
  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_fnptr_callee
// Needs: --goto-functions-only --clang-c-irep2-adjust-only
// RUN: --goto-functions-only --clang-c-irep2-adjust-only
// MODE: FUTURE
// EXPECT: FUNCTION_CALL:.*\*x\.update\(\)
// Vereda: verdict none
