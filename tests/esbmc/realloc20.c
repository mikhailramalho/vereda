#include <assert.h>
#include <stdlib.h>

unsigned __VERIFIER_nondet_uint();

void test_vector_growth()
{
  unsigned n = __VERIFIER_nondet_uint();
  __VERIFIER_assume(n > 0 && n <= 20); // symbolic but bounded

  int *vec = malloc(sizeof(int));
  unsigned size = 1;

  for (unsigned i = 0; i < n; ++i)
  {
    if (i == size)
    {
      size *= 2;
      vec = realloc(vec, size * sizeof(int));
      assert(vec != NULL);
    }
    vec[i] = i;
  }

  for (unsigned i = 0; i < n; ++i)
    assert(vec[i] == i);

  free(vec);
}

// Imported from ESBMC regression/esbmc/realloc20
// Needs: --force-realloc-success
// RUN: --force-malloc-success --force-realloc-success --function  test_vector_growth --unwind 21
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
