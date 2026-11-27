#include <assert.h>
#include <stdint.h>

// A packed struct has alignment 1, so its address must stay unconstrained:
// asserting any stronger alignment has to fail.

struct __attribute__((packed)) packed_struct
{
  char c;
  int i;
};
struct packed_struct g_packed;

int main()
{
  assert((uintptr_t)&g_packed % 4 == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/natural_alignment_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
