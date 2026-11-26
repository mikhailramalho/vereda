#include <pthread.h>
#include <stdio.h>

int x = 0;

/* The read of x is an argument to printf, which the frontend lowers to an
 * OTHER instruction rather than a call -- so the read reaches MPOR's access
 * sets only via execution_statet::symex_printf. Without it the reader looks
 * independent of the writer and the sleep set never wakes. */
void *reader(void *arg)
{
  printf("%d\n", x);
  return 0;
}

void *writer(void *arg)
{
  x = 1;
  return 0;
}

int main(void)
{
  pthread_t a, b;
  pthread_create(&a, 0, reader, 0);
  pthread_create(&b, 0, writer, 0);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_6831_sleep_sets_fail
// Needs: --data-race-check --sleep-sets
// RUN: --data-race-check --sleep-sets
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^Schedule reduction: peak_threads 3, schedules_explored [0-9]+, pruned_by_mpor 0, pruned_by_hash 0, pruned_by_sleep [1-9][0-9]*, pruned_by_cs_bound 0$
// MODE: FUTURE
// Vereda: verdict none
