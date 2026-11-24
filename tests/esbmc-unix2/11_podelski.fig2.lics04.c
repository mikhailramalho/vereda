#include <pthread.h> 
int x, y;

void* thr1() {
  while (x>0 && y>0) {
    {
      x = x-1;
      y = x;
    } 
  }
}    

void* thr2() {
  while (x>0 && y>0) {
    {
      x = y-2;
      y = x+1;
    } 
  }
}

int main() {
  pthread_t t1, t2;
  pthread_create(&t1, NULL, thr1, NULL);
  pthread_create(&t2, NULL, thr2, NULL);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix2/11_podelski.fig2.lics04
// RUN: --unwind 1 --no-unwinding-assertions
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict UNKNOWN
