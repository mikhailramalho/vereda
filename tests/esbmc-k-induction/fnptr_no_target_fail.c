#include <assert.h>

extern _Bool __VERIFIER_nondet_bool(void);

static unsigned x;

static void bump(void)
{
  x++;
}

int main(void)
{
  void (*f)(void) = bump;
  while (__VERIFIER_nondet_bool())
    f();
  assert(x == 0);
  return 0;
}
// Imported from ESBMC regression/k-induction/fnptr_no_target_fail
// Needs: --k-induction
// RUN: --k-induction --max-k-step 4 --ub-pointer-check=false
// MODE: FUTURE
// EXPECT: Disabling inductive step
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
