#include <assert.h>

int main() {
  unsigned int a = 0x80000000; // Negative negation of the halfway point (overflow risk)
  unsigned int result = -a; 
  assert(result == 0x7FFFFFFF); // Incorrect expected result for unsigned negation
  return result;
}
// Imported from ESBMC regression/esbmc/github_2338_8
// RUN: --ub-signed-overflow-check --unsigned-overflow-check --ub-shift-check
// EXPECT: ^VERIFICATION FAILED$
