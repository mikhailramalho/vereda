#include <pthread.h>


pthread_mutex_t  ma, mb;
int data1, data2;

void * thread1(void * arg)
{  
  pthread_mutex_lock(&ma);
  data1++;
  pthread_mutex_unlock(&ma);

  pthread_mutex_lock(&ma);
  data2++;
  pthread_mutex_unlock(&ma);

  assert(0);
}


void * thread2(void * arg)
{  
  pthread_mutex_lock(&ma);
  data1+=5;
  pthread_mutex_unlock(&ma);

  pthread_mutex_lock(&ma);
  data2-=6;
  pthread_mutex_unlock(&ma);
}


int main()
{
  pthread_t  t1, t2;

  pthread_mutex_init(&ma, 0);
  pthread_mutex_init(&mb, 0);

  data1 = 10;
  data2 = 10;

  pthread_create(&t1, 0, thread1, 0);
  pthread_create(&t2, 0, thread2, 0);
  
  pthread_join(t1, 0);
  pthread_join(t2, 0);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/02_stateful01_02
// Needs: --context-bound 2
// RUN: --context-bound 2 -Wno-error=implicit-function-declaration
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
