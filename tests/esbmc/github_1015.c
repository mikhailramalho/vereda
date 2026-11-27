#include <stdint.h>

int main() {
  // Defining sorts ...
  uint64_t mask = (uint64_t)-1 >> (sizeof(uint64_t) * 8 - 64);
  uint64_t mask2 = (uint64_t)1 << (64 - 1);
  __VERIFIER_assert(mask == 18446744073709551615ULL); // Right shift should hold
  __VERIFIER_assert(mask2 == 9223372036854775808ULL); // Left shift should hold

  return 0;
}
// Imported from ESBMC regression/esbmc/github_1015
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
