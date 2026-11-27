// Bank Account Example extracted/modified from http://stormchecker.codeplex.com

#include <pthread.h>
#include <stdio.h>

#define TRUE 1
#define FALSE 0

void *malloc(unsigned size);

int __VERIFIER_nondet_int();
int x, y, z;

// Account structure
typedef struct {
  int balance;
  pthread_mutex_t lock;
} ACCOUNT, *PACCOUNT;

PACCOUNT create(int b) 
{
  PACCOUNT acc = (PACCOUNT) malloc(sizeof(ACCOUNT));
  acc->balance = b;
  pthread_mutex_init(&acc->lock, 0);
  return acc;
}

int read(PACCOUNT acc) 
{
 return acc->balance;
}

void deposit(PACCOUNT acc) 
{
  pthread_mutex_lock(&acc->lock);
  acc->balance = acc->balance + y;
  pthread_mutex_unlock(&acc->lock);
}

void withdraw(PACCOUNT acc) 
{
  int r;
  pthread_mutex_lock(&acc->lock);
  r = read(acc);
  acc->balance = r - z;
  pthread_mutex_unlock(&acc->lock);
}

// Harness

// Thread 1
void* deposit_thread(void* arg) 
{
  deposit(arg);
}

// Thread 2
void* withdraw_thread(void* arg) 
{
  withdraw(arg);
}

int main() 
{
  pthread_t t1, t2;
  PACCOUNT acc;

  // Initialization
  x = __VERIFIER_nondet_int(); //balance
  y = __VERIFIER_nondet_int(); //deposit
  z = __VERIFIER_nondet_int(); //withdraw
  acc = create(x);

  // Threads
  pthread_create(&t1, 0, deposit_thread, acc);
  pthread_create(&t2, 0, withdraw_thread, acc);

  assert(read(acc) == x + y - z);

  return 0;
}
// Imported from ESBMC regression/esbmc-unix/02_account_symbolic_06
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict UNKNOWN
