#include <pthread.h>

/* Two threads whose accesses to x and y conflict in both directions, so the
 * schedule space is large enough for the partial-order reduction to matter,
 * and small enough to enumerate in about a second.
 *
 * main returns without joining, so interleavings are still generated after
 * its thread has ended -- the case #4584 was about. */

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
// Imported from ESBMC regression/esbmc-unix/interleaving_count_no_por
// Needs: --context-bound 3 --all-runs --no-por
// RUN: --unwind 3 --context-bound 3 --all-runs --no-por
// MODE: FUTURE
// EXPECT: ^Number of generated interleavings: 61$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
