// EXPECT: ^VERIFICATION UNKNOWN: unsupported: call to pthread_create at
// The thread function never ran, so its failed assertion was missed and the
// program was proved (#112). Until threads are modelled, a reached
// pthread_create gives UNKNOWN.
#include <assert.h>
#include <pthread.h>
void *f(void *a) {
  assert(0);
  return 0;
}
int main(void) {
  pthread_t t;
  pthread_create(&t, 0, f, 0);
  pthread_join(t, 0);
  return 0;
}
