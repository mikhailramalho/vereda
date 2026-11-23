// W1-loc spike Phase C (esbmc/esbmc#4715): pins the targets.gotos/targets.labels
// rollback in convert_function. A native attempt that consumes a code_goto2t
// pushes an entry holding an ITERATOR INTO ITS OWN goto_programt; if a later
// statement forces the whole function back to goto_convert_rec, that program is
// discarded and the iterator dangles. finish_gotos then dereferences it after
// the fallback rebuilds the body -- a segfault, not merely stale state.
//
// The label handler no longer declines under --error-label (it reproduces the
// ASSERT(false) since #4715's error-label arm), so this input is now converted
// natively end to end and pins the label/goto pair on the native path rather
// than the fallback ordering. The rollback itself is NOT discriminated by any
// test here: removing `targets = targets_before` in convert_function leaves
// every case green, because the failure mode is a dangling iterator read --
// latent UB that needs a sanitizer build to observe, not a crash.
#include <assert.h>

int g;

int f(int a)
{
  if (a > 0)
    goto ERROR;
  return 0;
ERROR:
  g = 1;
  return 1;
}

int main(void)
{
  assert(f(1) == 1);
  assert(g == 1);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_4715_irep2_native_body_goto_rollback_01
// Needs: --error-label ERROR --goto-functions-only
// RUN: --error-label ERROR --goto-functions-only
// MODE: FUTURE
// EXPECT: // Labels: ERROR
// EXPECT: ASSERT
// Vereda: verdict none
