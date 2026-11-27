// sv-benchmarks verifythis/duplets reduced. finddup's return block joins
// one arrival per pair i < j < 92, 4,186 of them, each writing its i to
// main's i. Joined as a chain of one ite per arrival, Bitwuzla pushed the
// extract of a stored byte down the chain past its limit of 4096 nested
// rewrites and answered sat: a counterexample where finddup returned i = 0,
// j = 2 and the check still failed.
// RUN: --unwind 92 --no-unwinding-assertions
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <assert.h>
#include <stdlib.h>
extern int __VERIFIER_nondet_int(void);

static void check(int cond) { assert(cond); }

static void mkdup(int *a, int n) {
  int i = __VERIFIER_nondet_int();
  int j = __VERIFIER_nondet_int();
  if (i < 0 || i >= n || j < 0 || j >= n || i == j)
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
