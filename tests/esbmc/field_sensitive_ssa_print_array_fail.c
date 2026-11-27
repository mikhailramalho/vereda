#include <assert.h>
#include <stdlib.h>

struct S
{
  int x;
  long pair[2];
};

int main(void)
{
  struct S t;
  t.x = 1;
  t.pair[0] = 6;
  t.pair[1] = 7;
  struct S *p = calloc(1, sizeof(struct S) + sizeof(long));
  if (!p)
    return 0;
  *p = t;
  struct S back = *p;
  assert(back.pair[1] != 7);
  return 0;
}
// Imported from ESBMC regression/esbmc/field_sensitive_ssa_print_array_fail
// RUN: -m64 --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^  back = \{ \.x=1, \.pair=\(signed long int \[2\]\)0x70000000000000006 \}$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict FAILED
