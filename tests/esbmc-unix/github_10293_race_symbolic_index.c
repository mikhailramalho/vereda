// #10293: the second write indexes by a tail that symex leaves symbolic.
#include <pthread.h>

typedef struct
{
  int element[20];
  int head;
  int tail;
  int amount;
} QType;

QType queue;

void *t1(void *arg)
{
  queue.element[queue.tail] = 3;
  queue.amount++;
  if (queue.tail == 20)
    queue.tail = 1;
  else
    queue.tail++;
  queue.element[queue.tail] = 4;
  queue.amount++;
  return 0;
}

int main()
{
  pthread_t id;
  pthread_create(&id, 0, t1, 0);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_10293_race_symbolic_index
// Needs: --data-races-check-only
// RUN: --data-races-check-only --incremental-bmc --max-k-step 2 --z3
// EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none
