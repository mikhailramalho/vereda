// A loop whose condition calls a function gets branching waypoints that say
// whether the condition held, not the opposite.
extern int __VERIFIER_nondet_int(void);

int main(void)
{
  int a[2];
  int i = 0;
  while (__VERIFIER_nondet_int())
    i++;
  a[i] = 1;
  return 0;
}
// Imported from ESBMC regression/esbmc/witness_branching_loop_call_cond
// Needs: --witness-output-yaml -
// RUN: --unwind 3 --no-unwinding-assertions --witness-output-yaml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: (?s)value: "true".*value: "true".*value: "false".*type: "target"
// EXPECT: (?s)\A(?!.*value: "false".*value: "true")
// Vereda: verdict none
