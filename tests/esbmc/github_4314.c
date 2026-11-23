#include <assert.h>
extern int __VERIFIER_nondet_int(void);

/* The assertion is violable at several unwindings, so an incremental run
   enumerates witnesses at more than one k. Without the bound in the header
   the blocks are indistinguishable from a repeat of the same one. */
int main()
{
  int n = __VERIFIER_nondet_int();
  __VERIFIER_assume(n >= 0 && n <= 12);
  int s = 0;
  for (int i = 0; i < n; i++)
    s += 1;
  assert(s != 2 && s != 3);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_4314
// Needs: --all-witnesses --keep-verified-claims --max-witnesses 2
// RUN: --incremental-bmc --all-witnesses --keep-verified-claims --max-witnesses 2
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: \[Counterexamples - [0-9]+ witnesses at k = 3\]
// EXPECT: \[Counterexamples - [0-9]+ witnesses at k = 4\]
// Vereda: verdict none
