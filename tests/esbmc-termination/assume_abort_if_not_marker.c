/* assume_abort_if_not(cond) marker pattern, lifted from the
 * seq-mthreaded SV-COMP benchmarks. The wrapper is defined as
 * `if (!cond) abort();` — the same shape as __VERIFIER_assert but
 * a different name. Aborts iff cond is false.
 *
 * insert_abort_call_markers_for_function recognises this wrapper
 * (alongside __VERIFIER_assert and a benchmark-overridden `assert`)
 * and emits ASSERT(cond) before each call. With the marker in
 * place, FC discharges the loop because the abort short-circuits
 * every iteration past the bound. Expected verdict:
 * VERIFICATION SUCCESSFUL. */

extern void abort(void);

void assume_abort_if_not(int cond)
{
  if (!cond)
    abort();
}

int main()
{
  int x = 0;
  while (1)
  {
    x++;
    assume_abort_if_not(x < 5);
  }
}
// Imported from ESBMC regression/termination/assume_abort_if_not_marker
// Needs: --force-realloc-success --termination --max-inductive-step 3
// RUN: --no-div-by-zero-check --force-malloc-success --force-realloc-success --no-align-check --k-step 2 --no-vla-size-check --32 --no-pointer-check --no-bounds-check --no-assertions --termination --max-inductive-step 3
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
