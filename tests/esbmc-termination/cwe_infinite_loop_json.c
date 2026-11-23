/* CWE-835 in the JSON report for a non-termination verdict.
 *
 * The synthetic trace that carries the non-termination verdict must reach
 * the --generate-json-report channel too: report.json records the verdict
 * as a violation whose assertion.cwe array contains 835.
 */
int main(void)
{
  while (1)
  {
  }
  return 0;
}
// Imported from ESBMC regression/termination/cwe_infinite_loop_json
// Needs: --termination --max-inductive-step 3 --generate-json-report CHECK_JSON report.json $[0].status == "violation" CHECK_JSON report.json $[0].steps[0].assertion.cwe[0] == 835 CHECK_JSON report.json $[0].steps[0].function == "main"
// RUN: --termination --k-step 2 --max-k-step 6 --max-inductive-step 3 --generate-json-report
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
