// Issue #10103: every --parallel-solving job shared the --smt-during-symex
// solver and crashed.
#include <assert.h>
extern int __VERIFIER_nondet_int(void);

int main()
{
  int N = 1;
  int src[N];
  int dst[N];
  src[0] = __VERIFIER_nondet_int();
  dst[0] = src[0];
  assert(dst[0] == src[0]);
  return 0;
}
// Imported from ESBMC regression/parallel-solving/github_10103_smt_during_symex
// Needs: --parallel-solving --smt-during-symex
// RUN: --parallel-solving --smt-during-symex
// MODE: FUTURE
// EXPECT: ^ERROR: --parallel-solving cannot be combined with --smt-during-symex$
// EXPECT: \A(?![\s\S]*VERIFICATION)[\s\S]*\Z
// Vereda: verdict none
