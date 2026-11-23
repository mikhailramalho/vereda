#include <assert.h>

int main() {
  unsigned int a = 0x80000000; // Power of two boundary case
  unsigned int result = -a; 
  return result;
}
// Imported from ESBMC regression/esbmc/github_2338_9
// RUN: --unsigned-overflow-check
// EXPECT: ^VERIFICATION FAILED$
