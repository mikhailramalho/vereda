int __VERIFIER_nondet_int(void);

int main(void)
{
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assert(x >= 0 || x < 0); // holds one
  __VERIFIER_assert(x == x); // holds two
  return 0;
}
// Imported from ESBMC regression/esbmc/github_6540_all_pass
// Needs: --smt-during-symex
// RUN: --multi-property --smt-during-symex
// MODE: FUTURE
// EXPECT: 2 passed
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
