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
// Imported from ESBMC regression/parallel-solving/worker_threads_negative
// Needs: --parallel-solving-threads -1
// RUN: --unwind 41 --parallel-solving-threads -1
// MODE: FUTURE
// EXPECT: ^Solving [0-9]+ VCCs in parallel, worker threads: 1$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
