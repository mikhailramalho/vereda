/* esbmc/esbmc#10920: as github_10920_array_stores, for struct members. */
struct pair
{
  int a;
  int b;
};
int __VERIFIER_nondet_int();
int main()
{
  struct pair p;
  p.a = __VERIFIER_nondet_int();
  p.b = __VERIFIER_nondet_int();
  __VERIFIER_assume(p.a >= 0 && p.a <= 3 && p.b >= 0 && p.b <= 3);
  __VERIFIER_assert(p.a + p.b != 3); // p.a + p.b != 3
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10920_struct_members
// Needs: --all-witnesses
// RUN: --all-witnesses --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^\[Counterexamples - 4 witnesses\]$
// EXPECT: enumeration stopped: UNSAT after 4 witnesses
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
