// p and q each take one of the two at every iteration, so the pointers'
// choices share their sides: a DAG of 30 joins, which the read-only check
// of the write must walk once per choice, not once per path (#509).
// RUN: --unwind 31
// EXPECT: ^  FAILED +\[main\.\S+\]  line 18  dereference failure: write access to const object `ro'$
// EXPECT: ^VERIFICATION FAILED$
extern int __VERIFIER_nondet_int(void);
static const int ro[4] = {1};
int rw[4], rw2[4];

int main(void) {
  int *p = (int *)ro, *q = rw;
  for (int i = 0; i < 30; i++) {
    int *t = __VERIFIER_nondet_int() ? p : q;
    q = __VERIFIER_nondet_int() ? q : (__VERIFIER_nondet_int() ? p : rw2);
    p = t;
  }
  *p = 1;
  return 0;
}
