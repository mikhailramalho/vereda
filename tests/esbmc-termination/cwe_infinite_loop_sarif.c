/* CWE-835 in the SARIF output for a non-termination verdict.
 *
 * A non-termination verdict has no counterexample trace (it is proven by
 * UNSAT at the loop exit marker), so ESBMC anchors a synthetic single-step
 * trace to the loop's marker to drive the SARIF emitter. The report must
 * carry CWE-835 both as a result-level `taxa` reference and in the
 * run-level `taxonomies` block.
 */
int main(void)
{
  while (1)
  {
  }
  return 0;
}
// Imported from ESBMC regression/termination/cwe_infinite_loop_sarif
// Needs: --termination --max-inductive-step 3 --sarif-output -
// RUN: --termination --k-step 2 --max-k-step 6 --max-inductive-step 3 --sarif-output -
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: "ruleId": "infinite-loop"
// EXPECT: "external/cwe/cwe-835"
// EXPECT: "helpUri": "https://cwe.mitre.org/data/definitions/835.html"
// EXPECT: "version": "4.20"
// Vereda: verdict none
