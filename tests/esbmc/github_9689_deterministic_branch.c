// #9689: branches fixed by the program text get no branching waypoint.
int sum(int n)
{
  if (n < 1)
    return 0;
  return n + sum(n - 1);
}

int main(void)
{
  int r = sum(5);
  __VERIFIER_assert(r != 15); // reached
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9689_deterministic_branch
// Needs: --witness-output-yaml -
// RUN: --witness-output-yaml - --no-unwinding-assertions --unwind 10
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: type: "target"
// EXPECT: (?s)\A(?!.*type: "branching")
// Vereda: verdict none
