#include <assert.h>

int main() {
  unsigned int a = 0;
  unsigned int result = -a; // Should remain 0 (no overflow)
  assert(!result);
  return result;
}

// Imported from ESBMC regression/esbmc/github_2338_1
// RUN: --ub-signed-overflow-check --unsigned-overflow-check --ub-shift-check --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
