// A store at a symbolic index after a summarised loop: a read below it
// goes through the store to the summary.
// EXPECT: ^Summarised 1 array loop\(s\)$
// EXPECT: ^VERIFICATION FAILED$
extern void __VERIFIER_assert(int);
extern unsigned __VERIFIER_nondet_uint(void);
#define N 100000
long a[N];
int main(void) {
  for (long i = 0; i < N; i++)
    a[i] = 2 * i;
  unsigned c = __VERIFIER_nondet_uint() % N;
  a[c] = -1;
  unsigned y = __VERIFIER_nondet_uint() % N;
  __VERIFIER_assert(y == c || a[y] == 2 * (long)y);
  __VERIFIER_assert(a[y] != -1);
  return 0;
}
