#include <pthread.h> 
int x, y;
int b1;

void thr1(){
  if (y){
    b1 = 0;
  }
}

void thr2(){
  x = 1;
  assert(x >= 1);
}

int main() {
  pthread_t t1, t2;
  pthread_create(&t1, NULL, thr1, NULL);
  pthread_create(&t2, NULL, thr2, NULL);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix2/11_lamport_2
// Needs: --no-por
// RUN: --unwind 1 --no-unwinding-assertions --no-por -Wno-error=implicit-function-declaration -Wno-error=incompatible-function-pointer-types
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
