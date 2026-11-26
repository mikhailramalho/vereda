#include <pthread.h>
#include <assert.h>
#include <time.h>
int x;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c = PTHREAD_COND_INITIALIZER;
struct timespec ts;
int main()
{
  pthread_mutex_lock(&m);
  pthread_cond_timedwait(&c, &m, &ts);
  pthread_mutex_unlock(&m);
  assert(x == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/partial_order_reject_cond
// Needs: --partial-order
// RUN: --partial-order
// MODE: FUTURE
// EXPECT: ^ERROR: --partial-order does not support a call to pthread_cond_timedwait yet, at file .*partial_order_reject_cond\.c line +11 column 3 function
// EXPECT: \A(?![\s\S]*VERIFICATION)[\s\S]*\Z
// Vereda: verdict none
