// C11 7.28 makes <uchar.h> define mbstate_t too, and ESBMC shadows <wchar.h>
// but not <uchar.h>, so both definitions meet in one translation unit here. A
// second one is a parse error rather than a wrong answer, and the assertion
// below would go nondet if mbsinit stopped binding to its model.
#include <uchar.h>
#include <wchar.h>

int main(void)
{
  mbstate_t st = {0};
  __VERIFIER_assert(mbsinit(&st)); // a zeroed state is the initial one
  return 0;
}
// Imported from ESBMC regression/esbmc/wchar_uchar_mbstate
// Needs: REQUIRES uchar_h
// RUN: --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict FAILED
