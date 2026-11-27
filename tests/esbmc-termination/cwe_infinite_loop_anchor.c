/* The CWE-835 anchor must name the user's loop, not ESBMC's own sources.
 *
 * goto_termination inserts a "termination per-loop marker" into library
 * helpers as well as user code, and __ESBMC_atexit_handler's
 * `while (atexit_count > 0)` loop (src/c2goto/library/stdlib.c) is linked
 * into every program. goto_functionst::function_map is ordered by mangled
 * id, so `c:@F@__ESBMC_atexit_handler` is visited before `c:@F@main`.
 * Selecting the first marker seen therefore anchored the SARIF/JSON/GraphML
 * location to stdlib.c inside ESBMC's install tree.
 *
 * Library markers must rank below user markers, so the anchor here is
 * main.c's loop exit.
 */
int main(void)
{
  while (1)
  {
  }
  return 0;
}
// Imported from ESBMC regression/termination/cwe_infinite_loop_anchor
// Needs: --termination --max-inductive-step 3 --sarif-output
// RUN: --termination --k-step 2 --max-k-step 6 --max-inductive-step 3 --sarif-output -
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: "ruleId": "infinite-loop"
// EXPECT: "uri": ".*cwe_infinite_loop_anchor\.c"
// Vereda: verdict none
