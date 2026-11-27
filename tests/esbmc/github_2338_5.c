#include <assert.h>

int main() {
  unsigned int a = 0x00000001;
  unsigned int result = -a;
  return result;
}

// Imported from ESBMC regression/esbmc/github_2338_5
// RUN: --ub-signed-overflow-check --unsigned-overflow-check --ub-shift-check --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
