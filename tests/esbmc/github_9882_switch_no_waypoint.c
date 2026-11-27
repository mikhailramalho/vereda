/* #9882: a switch gets no branching waypoint; validators cannot parse one. */
int __VERIFIER_nondet_int(void);

int main()
{
  int y = 0;
  switch (__VERIFIER_nondet_int())
  {
  case 1:
    y = 1;
    break;
  case 2:
  case 3:
    y = 2;
    break;
  case 7:
    y = 7;
    break;
  default:
    y = 3;
  }
  __VERIFIER_assert(y != 7); // case 7 taken
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9882_switch_no_waypoint
// Needs: --witness-output-yaml
// RUN: --witness-output-yaml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^ +type: "target"$
// EXPECT: (?s)\A(?!.*type: "branching")
// Vereda: verdict none
