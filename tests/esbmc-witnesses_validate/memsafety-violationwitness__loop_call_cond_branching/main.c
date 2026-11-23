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
// Imported from ESBMC regression/witnesses_validate/memsafety-violationwitness/loop_call_cond_branching
// Needs: --validate-violation-witness --witness witness.yml
// RUN: --unwind 3 --no-unwinding-assertions --validate-violation-witness --witness witness.yml
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
