#include <assert.h>

int main() {
  unsigned int a = 0x80000000; // Power of two boundary case
  unsigned int result = -a; 
  return result;
}
// Imported from ESBMC regression/esbmc/github_2338_9
// RUN: --ub-signed-overflow-check --unsigned-overflow-check --ub-shift-check
// EXPECT: ^VERIFICATION FAILED$
