/* Regression for #1520 (and #1471): an aggregate value (array / nested array /
 * struct, or any ESBMC-internal placeholder such as nondet_symbol / ARRAY_OF)
 * must never appear inside a GraphML witness assumption, because the SV-COMP
 * witness validators (CPAchecker, cpa-witness2test) cannot parse a brace
 * initialiser and reject the whole automaton.
 *
 * `status` is a nested array and the violating step reads a single scalar
 * element of it, so the witness should keep the scalar assumptions and drop
 * the unparseable `status == { { 0, ... } }`. The element is nondeterministic
 * so that the `x == 0;` expectation below is load-bearing: on a deterministic
 * element the assumption carries nothing a validator could not compute, and
 * constant propagation is free to drop it. */
unsigned char status[2][3];
unsigned char __VERIFIER_nondet_uchar(void);

int main()
{
  status[1][2] = __VERIFIER_nondet_uchar();
  unsigned char x = status[1][2];
  assert(x);
}
// Imported from ESBMC regression/esbmc/github_1520_witness_no_aggregate
// Needs: --witness-output-graphml
// RUN: --witness-output-graphml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: key="assumption">x == 0;
// EXPECT: (?s)\A(?!.*key="assumption">[^<]*[{}])
// Vereda: verdict none
