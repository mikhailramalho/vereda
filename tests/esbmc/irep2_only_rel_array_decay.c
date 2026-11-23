struct s
{
  int arr[4];
};

int main(void)
{
  struct s v;
  struct s *ptr = &v;

  /* An array-typed operand decays before the comparison; without that the two
     sides are an array and a pointer. */
  __VERIFIER_assert(ptr->arr != 0); // an array-typed member decays to a pointer
  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_rel_array_decay
// Needs: --goto-functions-only --clang-c-irep2-adjust-only
// RUN: --goto-functions-only --clang-c-irep2-adjust-only
// MODE: FUTURE
// EXPECT: ASSERT &ptr->arr\[0\] != 0
// Vereda: verdict none
