/* Issue #9656: the ranking-function verdict must write the correctness
 * witness like the forward-condition verdict does. */
extern int __VERIFIER_nondet_int(void);

int main()
{
  int x = __VERIFIER_nondet_int();
  while (x > 0)
    x--;
  return 0;
}
// Imported from ESBMC regression/termination/ranking_witness
// Needs: --termination --max-inductive-step 3 --witness-output-yaml witness.yml
// RUN: --termination --k-step 2 --max-inductive-step 3 --no-assertions --ub-pointer-check=false --no-bounds-check --witness-output-yaml witness.yml
// MODE: FUTURE
// EXPECT: ^Ranking function shows all executions terminate$
// EXPECT: ^Generating Correctness Yaml Witness for: .*ranking_witness\.c$
// Vereda: verdict none
