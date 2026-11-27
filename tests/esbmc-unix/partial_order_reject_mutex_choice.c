#include <pthread.h>
int __VERIFIER_nondet_int(void);
int x;
pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m2 = PTHREAD_MUTEX_INITIALIZER;
void *t(void *arg)
{
  pthread_mutex_t *l = __VERIFIER_nondet_int() ? &m1 : &m2;
  pthread_mutex_lock(l);
  x = 1;
  pthread_mutex_unlock(l);
  return 0;
}
int main()
{
  pthread_t id;
  pthread_create(&id, 0, t, 0);
  pthread_join(id, 0);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/partial_order_reject_mutex_choice
// Needs: --partial-order
// RUN: --partial-order --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^ERROR: --partial-order needs a mutex that is a global known during symbolic execution, at file .*partial_order_reject_mutex_choice\.c line +9
// EXPECT: \A(?![\s\S]*VERIFICATION)[\s\S]*\Z
// Vereda: verdict none
