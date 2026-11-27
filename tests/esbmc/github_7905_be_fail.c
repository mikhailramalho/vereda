// #7905: on a big-endian target the high half of a lane comes first.
#include <assert.h>

typedef int v4i __attribute__((__vector_size__(16)));
typedef short v8s __attribute__((__vector_size__(16)));

int main(void)
{
  v4i s = {0x00020001, 0, 0, 0};
  v8s h = (v8s)s;
  assert(h[0] != 2);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7905_be_fail
// Needs: --big-endian
// RUN: --big-endian --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
