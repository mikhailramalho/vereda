#include <assert.h>
#include <pthread.h>
void *t1(void *arg) {
  int *ptr = arg;
  *ptr=1;
  assert(*ptr==1);
  return 0;
}
int main() {
  int *p, incr=0;
  pthread_t id;
  p = &incr;
  pthread_create(&id, NULL, t1, &incr);
  assert(*p==1);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_152_fail2
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict UNKNOWN
