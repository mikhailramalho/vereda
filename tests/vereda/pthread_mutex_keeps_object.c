// EXPECT: ^VERIFICATION SUCCESSFUL$
// The mutex calls write only the mutex, so the counter next to it keeps
// its value.
#include <assert.h>
#include <pthread.h>
struct counter {
  pthread_mutex_t m;
  int n;
};
int main(void) {
  struct counter c = {.n = 1};
  pthread_mutex_init(&c.m, 0);
  pthread_mutex_lock(&c.m);
  ++c.n;
  pthread_mutex_unlock(&c.m);
  pthread_mutex_destroy(&c.m);
  assert(c.n == 2);
  return 0;
}
