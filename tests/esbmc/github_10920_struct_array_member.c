/* esbmc/esbmc#10920: an array inside a struct input is blocked element by
   element too. */
struct s
{
  int v[2];
};
int main()
{
  struct s p;
  __VERIFIER_assume(p.v[0] >= 0 && p.v[0] <= 3 && p.v[1] >= 0 && p.v[1] <= 3);
  __VERIFIER_assert(p.v[0] + p.v[1] != 3); // p.v[0] + p.v[1] != 3
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10920_struct_array_member
// Needs: --all-witnesses
// RUN: --all-witnesses
// MODE: FUTURE
// EXPECT: ^\[Counterexamples - 4 witnesses\]$
// EXPECT: enumeration stopped: UNSAT after 4 witnesses
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
