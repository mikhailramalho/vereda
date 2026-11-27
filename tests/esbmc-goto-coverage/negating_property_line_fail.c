#include <assert.h>
int __VERIFIER_nondet_int(void);
int main(void)
{
  int value = __VERIFIER_nondet_int();
  __VERIFIER_assume(value == 1);
  assert(value == 1);
  assert(value == 1);
}
// Imported from ESBMC regression/goto-coverage/negating_property_line_fail
// Needs: --negating-property main:8
// RUN: --negating-property main:8 --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: line 8
// Vereda: verdict none
