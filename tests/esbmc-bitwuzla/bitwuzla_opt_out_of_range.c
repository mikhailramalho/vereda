// --bitwuzla-opt reaches Bitwuzla's own option parser, which rejects an
// out-of-range value before the solver is built.
int __VERIFIER_nondet_int(void);

int main()
{
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assert(x != 1234); // x can be anything
  return 0;
}
// Imported from ESBMC regression/bitwuzla/bitwuzla_opt_out_of_range
// Needs: --bitwuzla-opt rewrite-level=9
// RUN: --bitwuzla --bitwuzla-opt rewrite-level=9 --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^ERROR: Bitwuzla error encountered$
// EXPECT: invalid value '9' for numeric option '--rewrite-level', maximum is 2
// Vereda: verdict none
