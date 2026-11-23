#include<pthread.h>
#include<assert.h>
#include<stdlib.h>
void a(int b) {
  if (!b)
    abort();
}
int e, g = 0;
void *thread2() {
  a(e == 0);
  if (g)
    assert(0);
  return NULL;
}
int main() {
  pthread_t t,t1;
  pthread_create(&t1, 0, thread2, 0);
  e = 1;
  g = 1;
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_6831_hashing_unsound_mpor
// Needs: --state-hashing
// RUN: --state-hashing
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
