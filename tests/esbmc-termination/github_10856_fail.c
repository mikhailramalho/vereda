/* #10856: memcpy's loop is skipped, and the loop after it never exits. */
#include <string.h>

int main(void)
{
  char d[4];
  memcpy(d, 0, 0);
  unsigned x = 1;
  while (x != 0)
    x |= 1;
  return 0;
}
// Imported from ESBMC regression/termination/github_10856_fail
// Needs: --termination --k-step 2 --max-inductive-step 3
// RUN: --termination --k-step 2 --max-k-step 40 --max-inductive-step 3
// MODE: FUTURE
// EXPECT: ^Recurrent state shows a non-terminating execution.*$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
