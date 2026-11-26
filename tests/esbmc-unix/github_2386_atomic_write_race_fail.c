/* esbmc/esbmc#2386: the atomic write to x is not synchronised with main's
 * plain read of x. */
#include <pthread.h>

extern void __VERIFIER_atomic_begin(void);
extern void __VERIFIER_atomic_end(void);

int x;

void *thread(void *arg)
{
  __VERIFIER_atomic_begin();
  x = 1;
  __VERIFIER_atomic_end();
  return 0;
}

int main(void)
{
  pthread_t t;
  pthread_create(&t, 0, thread, 0);
  int r = x;
  (void)r;
  pthread_join(t, 0);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_2386_atomic_write_race_fail
// Needs: --data-race-check --context-bound 2
// RUN: --data-race-check --context-bound 2
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^.*R/W data race on c:@x$
// MODE: FUTURE
// Vereda: verdict none
