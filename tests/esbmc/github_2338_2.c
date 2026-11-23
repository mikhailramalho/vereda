#include <assert.h>

int main() {
  unsigned int a = 0xFFFFFFFF;
  unsigned int result = -a;
  return result;
}

// Imported from ESBMC regression/esbmc/github_2338_2
// RUN: --unsigned-overflow-check
// EXPECT: ^VERIFICATION FAILED$
