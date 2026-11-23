int __VERIFIER_nondet_int(void);

int main(void)
{
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assert(x >= 0 || x < 0); // holds
  __VERIFIER_assert(x != 42); // may fail
  return 0;
}
// Imported from ESBMC regression/esbmc/github_6540_last
// Needs: --smt-during-symex
// RUN: --multi-property --smt-during-symex
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
