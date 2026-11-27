/* Mutual recursion is out of scope for the per-function ranking prover
 * (a measure must decrease around the whole call cycle, not within one
 * function). analyze_recursion classifies f<->g as `unsupported` and the
 * ranking check returns UNKNOWN, so this non-terminating program is NOT
 * wrongly certified as terminating.
 *
 * Expected verdict: VERIFICATION UNKNOWN (never SUCCESSFUL). */
extern int __VERIFIER_nondet_int(void);
void g(int x);
void f(int x) { if (x > 0) g(x + 1); }
void g(int x) { if (x > 0) f(x + 1); }
int main()
{
  f(__VERIFIER_nondet_int());
  return 0;
}
// Imported from ESBMC regression/termination/ranking_mutual_recursion
// Needs: --force-realloc-success --termination --max-inductive-step 3
// RUN: --ub-div-by-zero-check=false --force-malloc-success --force-realloc-success --no-align-check --k-step 2 --vla-size-check=false -m32 --ub-pointer-check=false --no-bounds-check --no-assertions --termination --max-inductive-step 3
// EXPECT: ^VERIFICATION UNKNOWN$
// MODE: FUTURE
// Vereda: verdict none
