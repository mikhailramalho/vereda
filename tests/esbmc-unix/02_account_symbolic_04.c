#include <pthread.h>
#include <assert.h>

int __VERIFIER_nondet_int();

pthread_mutex_t m, n;
int x, y, z, balance;
_Bool deposit_done=0, withdraw_done=0;

void *deposit(void *arg) 
{
  pthread_mutex_lock(&m); //lock 1
  balance = balance + y;
  deposit_done=1;
  pthread_mutex_unlock(&m); //unlock 1
}

void *withdraw(void *arg) 
{
  pthread_mutex_lock(&n);  //lock 2
  balance = balance - z;
  withdraw_done=1;
  pthread_mutex_unlock(&n);
}

void *check_result(void *arg) 
{
  pthread_mutex_lock(&m); //lock 3
  pthread_mutex_lock(&n); //lock 4
  pthread_mutex_unlock(&m); //unlock 2
  pthread_mutex_unlock(&n); //unlock 3
}

int main() 
{
  pthread_t t1, t2, t3;

  pthread_mutex_init(&m, 0);
  pthread_mutex_init(&n, 0);

  x = __VERIFIER_nondet_int();
  y = __VERIFIER_nondet_int();
  z = __VERIFIER_nondet_int();
  balance = x;

  pthread_create(&t1, 0, deposit, 0);
  pthread_create(&t2, 0, withdraw, 0);

  pthread_join(t1, NULL);
  pthread_join(t2, NULL);

  return 0;
}
// Imported from ESBMC regression/esbmc-unix/02_account_symbolic_04
// Needs: --context-bound 1
// RUN: --no-slice --context-bound 1
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
