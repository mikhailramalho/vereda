#include <pthread.h>

/* Two threads whose accesses to x and y conflict in both directions, so the
 * reduction has something to prune and the counters have something to report.
 * main returns without joining, so schedules continue past its thread ending. */

static int x, y;

static void *ta(void *arg)
{
  x = 1;
  y = x + 1;
  return 0;
}

static void *tb(void *arg)
{
  y = 2;
  x = y + 1;
  return 0;
}

int main(void)
{
  pthread_t a, b;
  pthread_create(&a, 0, ta, 0);
  pthread_create(&b, 0, tb, 0);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/schedule_prune_counters
// Needs: --context-bound 3
// RUN: --unwind 3 --context-bound 3
// MODE: FUTURE
// EXPECT: ^Schedule reduction: peak_threads 3, schedules_explored 40, pruned_by_mpor 38, pruned_by_hash 0, pruned_by_sleep 0, pruned_by_cs_bound \d+$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
