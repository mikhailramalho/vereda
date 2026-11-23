#include <assert.h>

int main() {
  unsigned int a = 0x00000001;
  unsigned int result = -a;
  return result;
}

// Imported from ESBMC regression/esbmc/github_2338_5
// RUN: --unsigned-overflow-check
// EXPECT: ^VERIFICATION FAILED$
