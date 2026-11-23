/* Self-deadlock: a spinlock is not recursive. */
#include <pthread.h>
pthread_spinlock_t sl;
int main(void)
{
  pthread_spin_init(&sl, 0);
  pthread_spin_lock(&sl);
  pthread_spin_lock(&sl);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_6478-spinlock_fail
// Needs: --deadlock-check
// RUN: --unwind 4 --deadlock-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: Deadlocked state in pthread_spin_lock
// Vereda: verdict none
