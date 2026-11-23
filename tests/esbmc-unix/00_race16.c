#include <pthread.h>

int data;

void * thread_routine(void * arg)
{
  data++;
}


int main()
{
  pthread_t  t1, t2;

  pthread_create(&t1, 0, thread_routine, 0);
  pthread_create(&t2, 0, thread_routine, 0);
  
  pthread_join(t1, 0);
  pthread_join(t2, 0);

  return 0;
}
// Imported from ESBMC regression/esbmc-unix/00_race16
// Needs: --data-races-check --context-bound 3
// RUN: --data-races-check --context-bound 3
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
