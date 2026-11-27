/* Companion to github_1520_witness_no_aggregate: confirms the aggregate filter
 * does not over-prune. A plain scalar input assumption must still be emitted in
 * the GraphML witness so validators can replay the violating value. */
int main()
{
  int x;
  if (x == 5)
    assert(0);
}
// Imported from ESBMC regression/esbmc/github_1520_witness_scalar_kept
// Needs: --witness-output-graphml -
// RUN: --witness-output-graphml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: key="assumption">x == 5;
// Vereda: verdict none
