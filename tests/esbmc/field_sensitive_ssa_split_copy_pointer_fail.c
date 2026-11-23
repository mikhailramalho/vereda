#include <assert.h>
#include <stdlib.h>

struct V
{
  int *buf;
  int size;
};

int main(void)
{
  struct V s;
  s.buf = malloc(2 * sizeof(int));
  s.size = 2;
  struct V out = s;
  int n = 0;
  for (int i = 0; i < out.size; i++)
    n++;
  assert(n == 3);
  return 0;
}
// Imported from ESBMC regression/esbmc/field_sensitive_ssa_split_copy_pointer_fail
// Needs: --field-sensitive-ssa
// RUN: --field-sensitive-ssa --unwind 6
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
