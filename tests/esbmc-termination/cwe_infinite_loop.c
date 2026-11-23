/* CWE-835 (Loop with Unreachable Exit Condition / infinite loop).
 *
 * A bare `while (1) {}` comes back to the same state, so
 * --termination refutes the termination property with the
 * recurrent-state check. The non-termination
 * verdict now carries CWE-835 on a `CWE: CWE-835` line immediately after
 * the verdict comment, matching the annotation ESBMC emits for every
 * other property-violation kind.
 */
int main(void)
{
  while (1)
  {
  }
  return 0;
}
// Imported from ESBMC regression/termination/cwe_infinite_loop
// Needs: --termination --max-inductive-step 3
// RUN: --termination --k-step 2 --max-k-step 6 --max-inductive-step 3
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^Recurrent state shows a non-terminating execution.*$
// EXPECT: ^CWE: CWE-835$
