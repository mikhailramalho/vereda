#include <assert.h>
#include <pthread.h> 

int X = 0;
int Y = 0;

void* thread() {
  if(Y)
    Y = 1;
}

int main() {
  pthread_t t1;
  pthread_create(&t1, NULL, thread, NULL);
  X = 1;
  assert(X==1);
  return 0;
}

// Imported from ESBMC regression/esbmc-unix2/github_1213
// RUN: --no-slice
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
