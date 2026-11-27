#include <pthread.h>
#include <assert.h>

pthread_mutex_t m;
int counter;

void *worker(void *arg)
{
  pthread_mutex_lock(&m);
  counter++;
  assert(counter > 0);
  pthread_mutex_unlock(&m);
  return NULL;
}

int main()
{
  pthread_t t1, t2;

  pthread_mutex_init(&m, NULL);
  counter = 0;

  pthread_create(&t1, NULL, worker, NULL);
  pthread_create(&t2, NULL, worker, NULL);
  pthread_join(t1, NULL);
  pthread_join(t2, NULL);

  assert(counter == 2);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_6831_falsify_prepass_safe
// Needs: --context-bound 2 --falsify-context-bound 3
// RUN: --context-bound 2 --falsify-context-bound 3
// MODE: FUTURE
// EXPECT: ^Falsifying with context bound 3$
// EXPECT: Falsifying with context bound 1\n(?:(?!VERIFICATION SUCCESSFUL)[\s\S])*Falsifying with context bound 3
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
