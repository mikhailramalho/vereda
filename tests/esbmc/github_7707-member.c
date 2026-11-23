#include <stdint.h>

/* The counterpart to github_7707: a direct packed-member read compiles to a
 * byte-wise access and is not UB, so mode.unaligned must keep it quiet. */

struct __attribute__((packed)) S
{
  char a;
  char pad[7];
  uint64_t b;
};

int main(void)
{
  struct S sp;
  struct S *q = &sp;
  uint64_t z = q->b;
  (void)z;
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7707-member
// EXPECT: ^WARNING: not checking alignment for access to packed struct struct S$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^WARNING: not checking alignment for access to packed struct struct S$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
