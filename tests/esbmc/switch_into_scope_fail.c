#include <assert.h>

int main()
{
  int r = 0;
  for (int pass = 0; pass < 2; ++pass)
  {
    switch (pass)
    {
    case 0:
    {
      int i = 5;
      continue;
    case 1:
      r = i;
    }
    }
  }
  // case 1 skips i's declaration, so i is indeterminate (C11 6.2.4p6)
  assert(r == 5);
}
// Imported from ESBMC regression/esbmc/switch_into_scope_fail
// RUN: --unwind 4 --no-unwinding-assertions
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
