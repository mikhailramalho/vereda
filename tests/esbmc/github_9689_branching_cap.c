/* Issue #9689: the YAML witness must not carry one branching waypoint per
   executed branch. */
int main(void)
{
  int s = 0;
  for (int i = 0; i < 1500; i++)
    if (i % 2)
      s++;
  __VERIFIER_assert(s == 750); // reached
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9689_branching_cap
// Needs: --witness-output-yaml
// RUN: --witness-output-yaml - --unwind 1600 --no-unwinding-assertions
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
