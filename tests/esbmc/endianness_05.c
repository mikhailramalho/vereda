#include <string.h>
#include <assert.h>

struct chars
{
  char fst;
  char snd;
};

int main()
{
  struct chars c = {0x01, 0x02};
  short s;
  memcpy(&s, &c, 2);
  /* expect s == 0x0102 under --big-endian; negate to force counterexample */
  assert(s != 0x0102);
  return 0;
}
// Imported from ESBMC regression/esbmc/endianness_05
// Needs: --big-endian
// RUN: --big-endian --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
