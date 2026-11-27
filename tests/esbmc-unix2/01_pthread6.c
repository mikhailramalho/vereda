#include <pthread.h>

int count;

void __VERIFIER_atomic_begin();
void __VERIFIER_atomic_end();

void *my_thread(void *arg)
{
  __VERIFIER_atomic_begin();
  count++;
  count--;
  __VERIFIER_atomic_end();
}

int main(void)
{
  pthread_t id;

  pthread_create(&id, NULL, my_thread, NULL);

  __VERIFIER_atomic_begin();
  __VERIFIER_assert(count==0); // property
  __VERIFIER_atomic_end();

  return 0;
}
// Imported from ESBMC regression/esbmc-unix2/01_pthread6
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict UNKNOWN
