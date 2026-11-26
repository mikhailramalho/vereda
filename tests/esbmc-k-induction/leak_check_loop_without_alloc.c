// The loop allocates nothing, so the leak check keeps the inductive step,
// which proves the program free of leaks for any bound.
#include <stdlib.h>
extern int __VERIFIER_nondet_int(void);

int main(void)
{
  int *p = malloc(sizeof(int));
  if (!p)
    return 0;
  *p = 0;
  while (__VERIFIER_nondet_int())
    *p = *p + 1;
  free(p);
  return 0;
}
// Imported from ESBMC regression/k-induction/leak_check_loop_without_alloc
// Needs: --k-induction --max-inductive-step 3 --memory-leak-check
// RUN: --k-induction --max-inductive-step 3 --memory-leak-check --no-assertions
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
