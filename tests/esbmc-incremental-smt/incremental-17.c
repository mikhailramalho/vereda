#include <assert.h>
int main()
{
  char ch = __VERIFIER_nondet_char();

  switch(ch)
  {
  case 'P':
  case 'p':
    assert(ch == 80 || ch == 112);

  default:;
  }
}
// Imported from ESBMC regression/incremental-smt/incremental-17
// Needs: --smt-symex-assert --smt-symex-guard
// RUN: --smt-symex-assert --smt-symex-guard
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
