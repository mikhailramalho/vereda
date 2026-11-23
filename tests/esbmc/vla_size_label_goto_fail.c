#include <assert.h>
int main(void)
{
  int n = 1, k = 0;
L:
  int a[n];
  if (k == 0)
  {
    k = 1;
    n = 2;
    goto L;
  }
  assert(sizeof(a) == 1 * sizeof(int)); /* re-reached with n == 2 */
  return 0;
}
// Imported from ESBMC regression/esbmc/vla_size_label_goto_fail
// RUN: --unwind 3
// EXPECT: ^VERIFICATION FAILED$
