#include <assert.h>
int main()
{
  int i, s = 0;
  for (i = 0; i < 4; i++)
  {
    if (i > 1)
      s += i;
    assert(s >= 0);
  }
  assert(i == 4);
  return 0;
}
// Imported from ESBMC regression/goto-coverage/github_6387_6
// Needs: --condition-coverage --k-induction
// RUN: --multi-property --condition-coverage --k-induction --unwind 6
// MODE: FUTURE
// EXPECT: ^Total Conditions:  8$
// EXPECT: \A(?![\s\S]*VERIFICATION FAILED)[\s\S]*\Z
// Vereda: verdict none
