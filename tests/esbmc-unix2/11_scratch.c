#include <pthread.h> 
int g;

void thr() {
  int junk = 0;
  g = 0;
  junk++;
  junk++;
  junk++;
  g++;
  junk++;
  junk++;
  assert(g <=2);
  junk++;
  junk++;
  junk++;
  junk++;
  junk++;
}

int main() {
  pthread_t t1;
  pthread_create(&t1, NULL, thr, NULL);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix2/11_scratch
// RUN: --unwind 10
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict UNKNOWN
