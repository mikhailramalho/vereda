/* github #10174: the assertion after the loop is only reachable through the
 * loop's real exit edge. */
#include <assert.h>
int main()
{
  int d = 1, x, t;
top:
  if (d <= 3)
  {
    if (d <= 1)
    {
      x = 1;
    two:
      if (x >= 1)
      {
        t = 1;
      l:
        d = d + 1;
        goto top;
      b:
        t = 1;
        goto l;
      }
      else
      {
        goto b;
      }
    }
    else
    {
      x = 2;
      goto two;
    }
  }
  assert(d != 4);
  return 0;
}
// Imported from ESBMC regression/k-induction/github_10174_fail
// Needs: --k-induction
// RUN: --k-induction --max-k-step 8
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
