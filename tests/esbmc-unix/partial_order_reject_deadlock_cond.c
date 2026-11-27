/* #10990: with --deadlock-check, a wait that no signal ends is a deadlock, but
   --partial-order lets every wait wake at will. */
#include <pthread.h>
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c = PTHREAD_COND_INITIALIZER;
int main()
{
  pthread_mutex_lock(&m);
  pthread_cond_wait(&c, &m);
  pthread_mutex_unlock(&m);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/partial_order_reject_deadlock_cond
// Needs: --partial-order --deadlock-check
// RUN: --partial-order --deadlock-check
// MODE: FUTURE
// EXPECT: ^ERROR: --partial-order does not support a call to pthread_cond_wait_check yet, at file .*partial_order_reject_deadlock_cond\.c line +9 column 3 function
// EXPECT: \A(?![\s\S]*VERIFICATION)[\s\S]*\Z
// Vereda: verdict none
