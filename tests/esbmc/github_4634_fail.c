// Companion-fail for #4634.
//
// Confirms that gating `add_memory_leak_checks()` on the
// all-threads-terminal predicate does not suppress real leaks. The
// thread mallocs a block, stores the pointer only in a stack local,
// returns without freeing or exporting it. At the all-threads-terminal
// schedule the block is unreachable from any live pointer — a genuine
// `valid-memcleanup` violation.

#include <pthread.h>
#include <stdlib.h>

static void *t_fun(void *arg)
{
  (void)arg;
  void *p = malloc(16);
  (void)p;
  return NULL;
}

int main(void)
{
  pthread_t t1;
  pthread_create(&t1, NULL, t_fun, NULL);
  pthread_join(t1, NULL);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_4634_fail
// Needs: --memory-leak-check --no-reachable-memory-leak --no-abnormal-memory-leak --no-assertions
// RUN: --memory-leak-check --no-reachable-memory-leak --no-abnormal-memory-leak --no-assertions --incremental-bmc --k-step 2
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^.*forgotten memory.*$
// Vereda: verdict none
