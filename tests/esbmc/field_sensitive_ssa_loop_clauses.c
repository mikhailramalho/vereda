/* Objects named in a loop invariant or assigns clause stay whole; only u is
 * split. */
struct S
{
  int i;
  int n;
};

int main(void)
{
  struct S s, t, u;
  s.i = 0;
  s.n = 10;
  t.i = 1;
  t.n = 2;
  u.i = 3;
  u.n = 4;
  __ESBMC_loop_assigns(s.i, t.i);
  __ESBMC_loop_invariant(s.i >= 0 && s.i <= 10);
  while (s.i < s.n)
  {
    s.i++;
    t.i = s.i;
  }
  __VERIFIER_assert(s.i == 10 && t.n == 2 && u.i + u.n == 7); // post
  return 0;
}
// Imported from ESBMC regression/esbmc/field_sensitive_ssa_loop_clauses
// Needs: __ESBMC_loop_assigns, __ESBMC_loop_invariant
// MODE: FUTURE
// EXPECT: ^Field-sensitive SSA splits 1 struct object\(s\)$
// EXPECT: ^VERIFICATION SUCCESSFUL$
