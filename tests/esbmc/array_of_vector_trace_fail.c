#include <assert.h>
typedef int v4i __attribute__((__vector_size__(16)));
int main(void)
{
  v4i a[3] = {};
  for (int j = 0; j < 4; j++)
    a[2][j] = 20 + j;
  v4i x = a[2];
  assert(x[0] == 21);
  return 0;
}
// Imported from ESBMC regression/esbmc/array_of_vector_trace_fail
// RUN: --unwind 5
// EXPECT: ^  x = \{ 20, 21, 22, 23 \}$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^  x = \{ 20, 21, 22, 23 \}$
// MODE: FUTURE
// Vereda: verdict FAILED
