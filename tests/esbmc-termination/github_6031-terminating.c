extern int __VERIFIER_nondet_int(void);

// Companion to github_6031: a terminating program under the same flag
// combination (--termination --k-induction-parallel). The loop always
// terminates, so the sequential termination strategy (which --termination
// now routes to instead of the parallel driver) must report VERIFICATION
// SUCCESSFUL. This pins that the priority routing is not a blanket "always
// FAILED".
int main()
{
  int n = __VERIFIER_nondet_int();
  while (n < 10)
    n++;
  return 0;
}
// Imported from ESBMC regression/termination/github_6031-terminating
// Needs: --termination --k-induction-parallel
// RUN: --termination --k-induction-parallel
// MODE: FUTURE
// EXPECT: Both k-induction and --termination specified. Using --termination.
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
