#include <assert.h>

struct
{
  _ExtInt(17) field_1;
  int field_2;
} s;

int main()
{
  // Check that the field is at the correct alignment and can be dereferenced
  assert(*(&s.field_2) == *(int *)((void *)&s.field_1 + 4));
}
// Imported from ESBMC regression/esbmc/struct_extint_01
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
