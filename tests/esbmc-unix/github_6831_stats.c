#include <assert.h>
#include <pthread.h>

int x = 0;

void *bump(void *arg)
{
  x++;
  x++;
  return 0;
}

int main(void)
{
  pthread_t a, b;
  pthread_create(&a, 0, bump, 0);
  pthread_create(&b, 0, bump, 0);
  pthread_join(a, 0);
  pthread_join(b, 0);
  assert(x == 4);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_6831_stats
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: ^Schedule reduction: peak_threads 3, schedules_explored [0-9]+, pruned_by_mpor [1-9][0-9]*, pruned_by_hash 0, pruned_by_sleep 0, pruned_by_cs_bound 0$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict UNKNOWN
