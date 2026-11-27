// join_many_arrivals with a real bug: mkdup may write the same index twice,
// so with n = 2 there may be no duplicate and finddup returns 0.
// RUN: --unwind 92 --no-unwinding-assertions
// EXPECT: ^VERIFICATION FAILED$
#include <assert.h>
#include <stdlib.h>
extern int __VERIFIER_nondet_int(void);

static void check(int cond) { assert(cond); }

static void mkdup(int *a, int n) {
  int i = __VERIFIER_nondet_int();
  int j = __VERIFIER_nondet_int();
  if (i < 0 || i >= n || j < 0 || j >= n)
    abort();
  a[i] = 1;
  a[j] = 1;
}

static int finddup(int *a, int n, int *_i, int *_j) {
  for (int i = 0; i < n; i++)
    for (int j = i + 1; j < n; j++)
      if (a[i] == a[j]) {
        *_i = i;
        *_j = j;
        return 1;
      }
  return 0;
}

int main() {
  int n = __VERIFIER_nondet_int();
  if (n < 0 || n > 93)
    abort();
  int *a = calloc(n, sizeof(int));
  if (!a)
    abort();
  mkdup(a, n);
  int i, j;
  int r = finddup(a, n, &i, &j);
  check(r);
  check(0 <= i && i < n);
  check(0 <= j && j < n);
  return 0;
}
