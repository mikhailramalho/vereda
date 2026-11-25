/* A RETURN inside a loop body is a genuine loop exit and must get a
 * termination marker. The marker pass historically only marked
 * forward-GOTO exit edges, so `while (1) { x = nondet(); if (x == 0)
 * return 0; }` had no reachable marker on its real exit (the natural-
 * exit marker sits past the always-false `IF !1`), and IS reported
 * spurious non-termination.
 *
 * insert_markers_for_function now also collects RETURN instructions in
 * the loop body range and places an ASSERT(false) before each. Because
 * the pass runs per-function and only scans this loop's own body, the
 * RETURN is guaranteed to belong to the loop-owning function — a return
 * inside a callee invoked from the loop lives in a separate function
 * body and is never mistaken for a loop exit.
 *
 * The test inspects the transformed GOTO program (multi-line match):
 * the per-loop ASSERT(false) marker must appear immediately before the
 * in-loop `RETURN: 0`. */

extern int __VERIFIER_nondet_int(void);

int main()
{
  while (1)
  {
    int x = __VERIFIER_nondet_int();
    if (x == 0)
      return 0;
  }
}
// Imported from ESBMC regression/termination/return_exit_marker
// Needs: --termination --goto-functions-only
// RUN: --termination --32 --goto-functions-only
// MODE: FUTURE
// EXPECT: ASSERT 0 // termination per-loop marker\n.*\n.*RETURN: 0
// Vereda: verdict none
