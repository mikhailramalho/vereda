/* __VERIFIER_assert(cond) marker pattern, lifted from
 * sv-benchmarks/c/loops/sum03-1.c. The SV-COMP wrapper is defined
 * as `if (!cond) { reach_error(); abort(); }`; the call aborts iff
 * cond is false. The enclosing while(1) loop has no natural exit,
 * so the natural-exit marker placed by insert_markers_for_function
 * is statically unreachable and IS sees no marker at all — the
 * wrong-false branch of the canonical sum03-1 benchmark.
 *
 * insert_abort_call_markers_for_function recognises calls to
 * __VERIFIER_assert by name and emits ASSERT(cond) — a conditional
 * marker — immediately before each call. For integer cond (the
 * typical SV-COMP signature), we coerce to bool via `cond != 0`
 * so the solver sees a bool guard.
 *
 * The loop tracks sn = sum-of-2s while x counts iterations, and the
 * assertion encodes the invariant sn == 2x || sn == 0. The
 * invariant holds, the assertion never fires; FC closes at k=11.
 * Expected verdict: VERIFICATION SUCCESSFUL. */

extern void abort(void);
#include <assert.h>
void reach_error()
{
  assert(0);
}

void __VERIFIER_assert(int cond)
{
  if (!(cond))
  {
  ERROR:
  {
    reach_error();
    abort();
  }
  }
  return;
}
#define a (2)
extern unsigned int __VERIFIER_nondet_uint();

int main()
{
  int sn = 0;
  unsigned int loop1 = __VERIFIER_nondet_uint(), n1 = __VERIFIER_nondet_uint();
  unsigned int x = 0;

  while (1)
  {
    if (x < 10)
    {
      sn = sn + a;
    }
    x++;
    __VERIFIER_assert(sn == x * a || sn == 0);
  }
}
// Imported from ESBMC regression/termination/verifier_assert_marker
// Needs: --force-realloc-success --state-hashing --add-symex-value-sets --no-assertions --termination --max-inductive-step 3 REQUIRES arch32
// RUN: --no-div-by-zero-check --force-malloc-success --force-realloc-success --state-hashing --add-symex-value-sets --no-align-check --k-step 2 --floatbv --no-vla-size-check --32 --no-pointer-check --no-bounds-check --no-assertions --termination --max-inductive-step 3
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
