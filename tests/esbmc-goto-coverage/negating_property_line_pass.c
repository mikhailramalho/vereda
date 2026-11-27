#include <assert.h>
int __VERIFIER_nondet_int(void);
int main(void)
{
  int value = __VERIFIER_nondet_int();
  __VERIFIER_assume(value == 1);
  assert(value == 0);
  assert(value == 1);
}
// Imported from ESBMC regression/goto-coverage/negating_property_line_pass
// Needs: --negating-property main:7
// RUN: --negating-property main:7 --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
