#include <pthread.h>
#include <stdio.h>

pthread_mutex_t m;
int __VERIFIER_nondet_int();
int x, y, z, balance;
_Bool deposit_done=0, withdraw_done=0;

void *deposit(void *arg) 
{
  pthread_mutex_lock(&m);
  balance = balance + y;
  deposit_done=1;
  pthread_mutex_unlock(&m);
}

void *withdraw(void *arg) 
{
  pthread_mutex_lock(&m);
  balance = balance - z;
  withdraw_done=1;
  pthread_mutex_unlock(&m);
}

void *check_result(void *arg) 
{
  pthread_mutex_lock(&m);
  if (deposit_done && withdraw_done)
    assert(balance == (x + y) - z);
  pthread_mutex_unlock(&m);
}

int main() 
{
  pthread_t t1, t2;

  pthread_mutex_init(&m, 0);

  x = __VERIFIER_nondet_int();
  y = __VERIFIER_nondet_int();
  z = __VERIFIER_nondet_int();
  balance = x;

  assert(balance==x);

  pthread_create(&t1, 0, deposit, 0);
  pthread_create(&t2, 0, withdraw, 0);
  pthread_create(&t2, 0, check_result, 0);

  return 0;
}
// Imported from ESBMC regression/esbmc-unix/02_account_symbolic_02
// Needs: --context-bound 1
// RUN: --context-bound 1 -Wno-error=implicit-function-declaration
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
