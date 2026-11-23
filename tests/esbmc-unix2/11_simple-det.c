#include <pthread.h> 
int x=0, m=0;
int can_start = 0;
int done = 0;

void thr1() {
  { __VERIFIER_atomic_begin();
    can_start = can_start - 1;
    __VERIFIER_assume(can_start >= 0);
    __VERIFIER_atomic_end();
  }
  acquire(m); // m=0 /\ m'=1
  x = 2;
  release(m);
  done = done + 1;
}

void thr2() {
  { __VERIFIER_atomic_begin();
    can_start = can_start - 1;
    __VERIFIER_assume(can_start >= 0);
    __VERIFIER_atomic_end();
  }
  acquire(m); // m=0 /\ m'=1
  if (x == 0) { x = x + 2; }
  release(m);
  done = done + 1;
}

int main() {
  int x_out_1;
  int x_in_1 = x;
  can_start = 2; // fork thr1(); fork thr2();
  pthread_t t1, t2;
  pthread_create(&t1, NULL, thr1, NULL);
  pthread_create(&t2, NULL, thr2, NULL);
  __VERIFIER_assume(done >= 2); // join
  x_out_1 = x;
  x = x_in_1;
  can_start = 2; // fork thr1(); fork thr2();
  __VERIFIER_assume(done >= 4); // join
  assert(x_out_1 == x);
}

// Imported from ESBMC regression/esbmc-unix2/11_simple-det
// Needs: --context-bound 2
// RUN: --unwind 10 --context-bound 2 -Wno-error=implicit-function-declaration -Wno-error=incompatible-function-pointer-types
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
