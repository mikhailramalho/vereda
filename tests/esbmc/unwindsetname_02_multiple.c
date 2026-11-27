// Test --unwindsetname with multiple functions
// Each function needs different unwind bounds

#include <assert.h>

void func_ten() {
  int i, limit;
  int sum = 0;

  __VERIFIER_assume(limit == 10);

  for (i = 0; i < limit; i++) {
    sum += i;
  }

  assert(sum == 45);
}

void func_five() {
  int j, limit;
  int prod = 1;

  __VERIFIER_assume(limit == 5);

  for (j = 0; j < limit; j++) {
    prod *= 2;
  }

  assert(prod == 32);
}

void func_three() {
  int k, limit;
  int count = 0;

  __VERIFIER_assume(limit == 3);

  for (k = 0; k < limit; k++) {
    count++;
  }

  assert(count == 3);
}

int main() {
  func_ten();
  func_five();
  func_three();
  return 0;
}
// Imported from ESBMC regression/esbmc/unwindsetname_02_multiple
// RUN: --unwindsetname func_ten:0:11,func_five:0:6,func_three:0:4 --solver z3 --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none
// Needs: --unwindsetname func_ten:0:11,func_five:0:6,func_three:0:4
