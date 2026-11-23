#include <assert.h>

char *__VERIFIER_nondet_charp(void);
char b[64];

/* A nondet pointer pinned to b by an assume: its value set is unknown. */
int main(void)
{
  char *p = __VERIFIER_nondet_charp();
  __VERIFIER_assume(p == &b[0]);
  *p = 3;
  assert(b[0] == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/deref_unmodelled_write_assume
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^.*dereference failure: invalid pointer$
// Vereda: unmet EXPECT: ^.*dereference failure: invalid pointer$
// MODE: FUTURE
// Vereda: verdict FAILED
