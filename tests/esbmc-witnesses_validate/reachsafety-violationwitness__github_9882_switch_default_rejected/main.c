/* #9882: a default waypoint rules out case 1, so the witness is refuted. */
int __VERIFIER_nondet_int(void);

int main()
{
  int y = 0;
  switch (__VERIFIER_nondet_int())
  {
  case 1:
    y = 1;
    break;
  default:
    y = 3;
  }
  __VERIFIER_assert(y != 1); // case 1 taken
  return 0;
}
// Imported from ESBMC regression/witnesses_validate/reachsafety-violationwitness/github_9882_switch_default_rejected
// Needs: --validate-violation-witness --witness witness.yml
// RUN: --validate-violation-witness --witness witness.yml --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
