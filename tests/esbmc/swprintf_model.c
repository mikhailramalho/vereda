// C11 7.29.2.3: swprintf writes at most len wide characters including the
// terminating null and returns the count written, negative on error. The
// format is not interpreted (see headers/wchar.h), so the contents are nondet
// -- but the destination is written rather than left stale, which is the
// property a caller can rely on.
#include <wchar.h>
#include <assert.h>

int main(void)
{
  wchar_t buf[8];
  int r = swprintf(buf, 8, L"%d", 42);
  assert(r < 8);
  if (r >= 0)
    assert(buf[r] == 0);

  wchar_t small[2];
  int r2 = swprintf(small, 2, L"%ls", L"ab");
  assert(r2 < 2);
  if (r2 >= 0)
    assert(small[r2] == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/swprintf_model
// RUN: --unwind 10 --no-unwinding-assertions
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict FAILED
