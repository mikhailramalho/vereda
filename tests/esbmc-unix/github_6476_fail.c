#include <pthread.h>

int main(void)
{
  pthread_mutex_t m;
  pthread_mutex_init(&m, 0);
  pthread_mutex_lock(&m);
  pthread_mutex_destroy(&m);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_6476_fail
// Needs: --lock-order-check
// RUN: --unwind 4 --lock-order-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: attempt to destroy a locked mutex
// Vereda: verdict none
