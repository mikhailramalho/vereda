#include <assert.h>
int main() {
  int valve = __VERIFIER_nondet_int();  // environment
  int input = __VERIFIER_nondet_int();  // user input
  while (input) {
    if (!valve) {
      assert(valve == 0); // "privacy"
      valve = 1;
    }
    else if (valve == 1) {
      assert(valve > 0); // "performance"
       valve = 2;
    }
    else if (valve == 2) {
      assert(valve >= 2); // "vulnerability"
      valve = 3;
    }
  }
  assert(valve == 3);
}
// Imported from ESBMC regression/goto-coverage/branch_cov_12
// Needs: --branch-coverage-claims
// RUN: --branch-coverage-claims --unwind 2
// MODE: FUTURE
// EXPECT: ^Branches : 8
// EXPECT: ^Reached : 8
// EXPECT: ^Branch Coverage: 100
// EXPECT: ^COVERAGE ANALYSIS INCOMPLETE
// Vereda: verdict none
