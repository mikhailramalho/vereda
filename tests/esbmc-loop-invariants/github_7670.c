/* #7670: github_7585_weak in MSVC's assert spelling. The invariant leaves the
 * claim open either way, so the verdict must not depend on the host's
 * <assert.h>: unfolded the claim is constant false, and #7585's probe reads
 * that as the abstraction refuting it. */
void _wassert(const char *_Message, const char *_File, unsigned _Line);
#define ASSERT_MSVC(e) \
  (void)((!!(e)) || (_wassert(#e, __FILE__, (unsigned)__LINE__), 0))
int main(void)
{
  int i = 0, s = 0;
  __ESBMC_loop_invariant(i >= 0);
  while (i < 3)
  {
    s++;
    i++;
  }
  ASSERT_MSVC(s == 3);
  return 0;
}
// Imported from ESBMC regression/loop-invariants/github_7670
// Needs: __ESBMC_loop_invariant
// Needs: --loop-invariant-check
// RUN: --loop-invariant-check --multi-property
// MODE: FUTURE
// EXPECT: ^VERIFICATION UNKNOWN$
