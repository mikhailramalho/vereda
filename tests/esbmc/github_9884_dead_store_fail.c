// #9884: without propagation the branch reads a[0] directly. The slicer keeps
// that array version but drops the store a[0] = x as dead, so the branch must
// get no waypoint.
extern int __VERIFIER_nondet_int(void);

int main(void)
{
  int a[2];
  int y = 0;
  int x = __VERIFIER_nondet_int();
  a[0] = x;
  a[1] = x;
  if (a[0] > 5)
    y = 1;
  return a[1] + 1;
}
// Imported from ESBMC regression/esbmc/github_9884_dead_store_fail
// Needs: --witness-output-yaml -
// RUN: --overflow-check --no-assertions --witness-output-yaml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: type: "target"
// EXPECT: (?s)\A(?!.*type: "branching")
// Vereda: verdict none
