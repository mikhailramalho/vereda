// No witness for a failed assert: the witness states unreach-call, which
// only a call to reach_error violates.
// RUN: --witness -
// EXPECT: no witness: the violation \(__assert_fail\) is not a call to reach_error
// EXPECT-NOT: entry_type
// EXPECT: VERIFICATION FAILED
#include <assert.h>
int main(void) {
  assert(0);
  return 0;
}
