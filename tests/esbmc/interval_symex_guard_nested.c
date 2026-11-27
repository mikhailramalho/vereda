/* Nested loops: the inner variable j goes DEAD at the end of each
   outer iteration and is re-initialised by DECL+ASSUME on the next.
   Both loops are pruned by interval-based guard evaluation. */
int main()
{
  int i;
  __VERIFIER_assume(i >= 0);
  while (i < 3)
  {
    int j;
    __VERIFIER_assume(j >= 0);
    while (j < 2)
      ++j;
    __VERIFIER_assert(j >= 2); // inner bound must be met
    ++i;
  }
  __VERIFIER_assert(i >= 3); // outer bound must be met
  return 0;
}
// Imported from ESBMC regression/esbmc/interval_symex_guard_nested
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
