#include <pthread.h>
#include <assert.h>
int x, y;
pthread_rwlock_t rwlock;
void *writer(void *arg) {
  pthread_rwlock_wrlock(&rwlock);
  x = 3;
  pthread_rwlock_unlock(&rwlock);
  return 0;
}
void *reader(void *arg) {
  int l;
  pthread_rwlock_rdlock(&rwlock);
  l = x;
  y = l;
  if (x != 3 && y == x)
    assert(0);
  pthread_rwlock_unlock(&rwlock);
  return 0;
}
int main() {
  pthread_t t1, t2;
  pthread_create(&t1, 0, writer, 0);
  pthread_create(&t2, 0, reader, 0);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/01_read_write
// Needs: --context-bound 2 --no-por
// RUN: --context-bound 2 --no-por --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
