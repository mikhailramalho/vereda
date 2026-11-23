#include <assert.h>

int main() {
  unsigned int a = 0;
  unsigned int result = -a; // Should remain 0 (no overflow)
  assert(!result);
  return result;
}

// Imported from ESBMC regression/esbmc/github_2338_1
// RUN: --unsigned-overflow-check
// EXPECT: ^VERIFICATION SUCCESSFUL$
