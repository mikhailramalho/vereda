#include <assert.h>

unsigned __VERIFIER_nondet_uint(void);

int main()
{
  unsigned x = __VERIFIER_nondet_uint();
  __VERIFIER_assume(x < 100);
  for (unsigned i = 0; i < 40; ++i)
    assert(x != i + 1000);
  assert(x != 7);
  return 0;
}
// Imported from ESBMC regression/parallel-solving/worker_threads_option
// Needs: --parallel-solving-threads 2
// RUN: --unwind 41 --parallel-solving-threads 2
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
