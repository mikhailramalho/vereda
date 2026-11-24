#include <pthread.h>
int w, r, x, y;

void thr1() { //writer
  glb_init(w==0);
  glb_init(r==0);
  { __VERIFIER_atomic_begin();
    __VERIFIER_assume(w==0);
    __VERIFIER_assume(r==0);
    w = 1;
    __VERIFIER_atomic_end();
  }
  x = 3;
  w = 0;
}

void thr2() { //reader
  { __VERIFIER_atomic_begin();
    __VERIFIER_assume(w==0);
    r = r+1;
    __VERIFIER_atomic_end();
  }
  y = x;
  assert(y == x);
  r = r-1;
}

int main() {
  pthread_t t1, t2;
  pthread_create(&t1, NULL, thr1, NULL);
  pthread_create(&t2, NULL, thr2, NULL);
  return 0;
} 
// Imported from ESBMC regression/esbmc-unix2/11_read_write_lock
// RUN: --unwind 1 --no-unwinding-assertions
// MODE: THOROUGH
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict UNKNOWN
