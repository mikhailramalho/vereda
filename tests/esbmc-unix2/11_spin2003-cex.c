#include <pthread.h> 

int x=1, m=0; // the init values are ignored

void thr() {
  m = 0;
  acquire(m); // m=0 /\ m'=1
  x = 0;
  x = 1;
  assert(x>=1);
  release(m);
}

int main() {
  pthread_t t1;
  pthread_create(&t1, NULL, thr, NULL);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix2/11_spin2003-cex
// RUN: --unwind 1 --no-unwinding-assertions
// MODE: KNOWNBUG
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict UNKNOWN
