int __VERIFIER_nondet_int(void);
#include <assert.h>
#include <pthread.h>

pthread_spinlock_t s;

int main(void)
{
  pthread_spin_init(&s, 0);
  pthread_spin_lock(&s);
  if (__VERIFIER_nondet_int())
    pthread_spin_lock(&s);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/spinlock_deadlock_no_check_fail
// Needs: --deadlock-check
// RUN: --deadlock-check --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: Deadlocked state in pthread_spin_lock
// Vereda: verdict none
