/* Negative case for CWE-835: a loop that provably terminates must NOT be
 * flagged as an infinite loop. A linear ranking function bounds the loop,
 * so --termination proves the termination property and reports
 * VERIFICATION SUCCESSFUL with no CWE annotation.
 */
int main(void)
{
  int i = 0;
  while (i < 3)
    i++;
  return 0;
}
// Imported from ESBMC regression/termination/cwe_terminating_no_835
// Needs: --termination --max-inductive-step 3
// RUN: --termination --k-step 2 --max-k-step 6 --max-inductive-step 3
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
