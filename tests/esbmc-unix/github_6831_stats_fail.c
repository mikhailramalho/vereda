#include <assert.h>
#include <pthread.h>

int x = 0;

void *writer(void *arg)
{
  x = 1;
  return 0;
}

void *checker(void *arg)
{
  assert(x == 0);
  return 0;
}

int main(void)
{
  pthread_t a, b;
  pthread_create(&a, 0, writer, 0);
  pthread_create(&b, 0, checker, 0);
  pthread_join(a, 0);
  pthread_join(b, 0);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_6831_stats_fail
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^Schedule reduction: peak_threads 3, schedules_explored [0-9]+, pruned_by_mpor [0-9]+, pruned_by_hash 0, pruned_by_sleep 0, pruned_by_cs_bound 0$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict UNKNOWN
