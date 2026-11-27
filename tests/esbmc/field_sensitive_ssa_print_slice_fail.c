#include <assert.h>

struct S
{
  int a;
  void *p;
};

int main(void)
{
  struct S s;
  assert(s.a == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/field_sensitive_ssa_print_slice_fail
// RUN: --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^State 1 file [^\n]*field_sensitive_ssa_print_slice_fail\.c line 12 column
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict FAILED
