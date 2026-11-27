/* Two independent loops, each tracked separately by the interval domain. */
int main()
{
  int i, j;
  __VERIFIER_assume(i >= 0);
  __VERIFIER_assume(j >= 0);
  while (i < 3)
    ++i;
  while (j < 4)
    ++j;
  __VERIFIER_assert(i >= 3 && j >= 4); // both bounds must be met
  return 0;
}
// Imported from ESBMC regression/esbmc/interval_symex_guard_two_loops
// EXPECT: ^VERIFICATION SUCCESSFUL$
