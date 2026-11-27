#include <stdio.h>
#include <pthread.h> 

int  counter;
pthread_mutex_t g_mutex;


void* thread1(void* arg)
{ 
  pthread_mutex_lock(&g_mutex);
  counter++;
  pthread_mutex_unlock(&g_mutex);

  return NULL;
}


void* thread2(void* arg)
{ 
  pthread_mutex_lock(&g_mutex);
  counter++;
  pthread_mutex_unlock(&g_mutex);
  return NULL;
}



int main()
{
  pthread_t t1, t2, t3;

  pthread_mutex_init(&g_mutex, NULL);

  pthread_create(&t1, NULL, thread1, NULL);
  pthread_create(&t2, NULL, thread2, NULL);

  pthread_join(t1, NULL);
  pthread_join(t2, NULL);

  pthread_mutex_destroy(&g_mutex);

  return 0;
}


// Imported from ESBMC regression/esbmc-unix/02_simple1
// Needs: --context-bound 2
// RUN: --context-bound 2 --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
