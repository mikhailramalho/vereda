// #9222: the signed and unsigned checks use Z3's no-overflow predicates.
int __VERIFIER_nondet_int(void);
unsigned __VERIFIER_nondet_uint(void);
int main(void)
{
  int a = __VERIFIER_nondet_int(), b = __VERIFIER_nondet_int();
  unsigned u = __VERIFIER_nondet_uint(), v = __VERIFIER_nondet_uint();
  return a * b + (int)(u * v);
}
// Imported from ESBMC regression/z3/github_9222_formula
// RUN: --solver z3 --overflow-check --unsigned-overflow-check --dump-smt-only
// EXPECT: bvsmul_noovfl
// EXPECT: bvsmul_noudfl
// EXPECT: bvumul_noovfl
