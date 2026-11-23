// A loop whose condition assigns gets branching waypoints that say whether
// the condition held, not the opposite.
extern char __VERIFIER_nondet_char(void);

int main(void)
{
  char s[2];
  s[0] = __VERIFIER_nondet_char();
  s[1] = __VERIFIER_nondet_char();
  int n = 0;
  char c;
  while ((c = s[n++]) != 0)
    ;
  return 0;
}
// Imported from ESBMC regression/esbmc/witness_branching_loop_assign_cond
// Needs: --witness-output-yaml -
// RUN: --unwind 3 --no-unwinding-assertions --witness-output-yaml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: (?s)value: "true".*value: "true".*type: "target"
// EXPECT: (?s)\A(?!.*value: "false")
// Vereda: verdict none
