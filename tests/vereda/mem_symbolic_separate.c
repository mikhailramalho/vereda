// Symbolic-pointer reads and writes over separate/owned arrays and shared
// objects, plus a shared pointer-ite DAG exercising candidates() memoization.
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <string.h>

extern void __VERIFIER_assert(int);
extern void __VERIFIER_assume(int);
extern int __VERIFIER_nondet_int(void);
extern unsigned __VERIFIER_nondet_uint(void);

int main(void) {
  static unsigned char sep1[1024];
  static unsigned char sep2[1024];
  static unsigned char untouched[1024];
  unsigned char shared_buf[8] = {10, 11, 12, 13, 14, 15, 16, 17};

  memset(sep1, 1, sizeof(sep1));
  memset(sep2, 2, sizeof(sep2));
  memset(untouched, 7, sizeof(untouched));

  unsigned idx = __VERIFIER_nondet_uint();
  __VERIFIER_assume(idx < 8);
  int c1 = __VERIFIER_nondet_int();
  int c2 = __VERIFIER_nondet_int();

  // Pointer whose candidates are all in st.separate.
  unsigned char *p = c1 ? sep1 : sep2;
  p[idx] = 42;
  __VERIFIER_assert(p[idx] == 42);
  if (c1) {
    __VERIFIER_assert(sep1[idx] == 42);
    __VERIFIER_assert(sep2[idx] == 2);
  } else {
    __VERIFIER_assert(sep2[idx] == 42);
    __VERIFIER_assert(sep1[idx] == 1);
  }
  __VERIFIER_assert(untouched[idx] == 7);

  // Pointer mixing a separate object and a non-separate (shared) object.
  unsigned char *q = c2 ? sep1 : shared_buf;
  q[idx] = 99;
  __VERIFIER_assert(q[idx] == 99);
  if (c2) {
    __VERIFIER_assert(sep1[idx] == 99);
    __VERIFIER_assert(shared_buf[idx] == (unsigned char)(10 + idx));
  } else {
    __VERIFIER_assert(shared_buf[idx] == 99);
  }

  // Deep shared pointer-ite DAG (2^20 paths without memoization).
  unsigned char *dag = c1 ? sep1 : sep2;
  for (int i = 0; i < 20; ++i) {
    unsigned char *other = __VERIFIER_nondet_int() ? dag : sep2;
    dag = __VERIFIER_nondet_int() ? dag : other;
  }
  dag[0] = 55;
  __VERIFIER_assert(dag[0] == 55);
  __VERIFIER_assert(untouched[0] == 7);
  return 0;
}
