// Test that --unwindset overrides --unwindsetname
// Function bound says 3, but loop-specific bound says 10

#include <assert.h>

void test_priority() {
  int i, limit;
  int sum = 0;

  __VERIFIER_assume(limit == 10);

  // Loop 3 globally: --show-loops numbers the two C library loops first
  for (i = 0; i < limit; i++) {
    sum += i;
  }

  assert(sum == 45);  // Requires 10 unwinds, not 3
}

int main() {
  test_priority();
  return 0;
}
// Imported from ESBMC regression/esbmc/unwindsetname_03_priority
// RUN: --unwindsetname test_priority:0:4 --unwindset 3:11 --solver z3
// EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none
// Needs: --unwindset 3:11
