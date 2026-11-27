#include <assert.h>

int main() {
  unsigned int a = 0x40000000;
  unsigned int result = -a;
  return result;
}
// Imported from ESBMC regression/esbmc/github_2338_6
// RUN: --ub-signed-overflow-check --unsigned-overflow-check --ub-shift-check
// EXPECT: ^VERIFICATION FAILED$
