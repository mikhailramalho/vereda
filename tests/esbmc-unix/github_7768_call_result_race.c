#include <pthread.h>

/* The result of bump() is stored in a global, so the call itself accessed
 * shared memory and was instrumented inside an atomic block: bump() then ran
 * atomically and the race on `shared` inside it was never reported. */
int shared;
int results[2];

int bump(void)
{
  shared = shared + 1;
  return 0;
}

void *t(void *arg)
{
  results[(long)arg] = bump();
  return 0;
}

int main(void)
{
  pthread_t a, b;
  pthread_create(&a, 0, t, (void *)0);
  pthread_create(&b, 0, t, (void *)1);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_7768_call_result_race
// Needs: --data-races-check-only --no-assertions --context-bound 2
// RUN: --data-races-check-only --no-assertions --context-bound 2
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^.*W/W data race on c:@shared$
// Vereda: verdict none
