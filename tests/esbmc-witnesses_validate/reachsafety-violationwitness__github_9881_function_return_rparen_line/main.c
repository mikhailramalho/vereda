/* #9881: a function_return waypoint names a call by the line of its ')'. */
extern int __VERIFIER_nondet_int(void);
int value;
int main(void)
{
  // clang-format off
  value = __VERIFIER_nondet_int(
  );
  // clang-format on
  __VERIFIER_assert(value != 1); // target
}
// Imported from ESBMC regression/witnesses_validate/reachsafety-violationwitness/github_9881_function_return_rparen_line
// Needs: --validate-violation-witness --witness witness.yml
// RUN: -m32 --ub-pointer-check=false --validate-violation-witness --witness witness.yml --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none
