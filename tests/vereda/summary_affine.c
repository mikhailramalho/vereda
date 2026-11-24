// A loop that writes an affine value of its index to every element is
// replaced by its effect: a read of the array reduces to the value.
// EXPECT: ^Summarised 1 array loop\(s\)$
// EXPECT-NOT: Unwinding loop
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern void __VERIFIER_assert(int);
extern unsigned __VERIFIER_nondet_uint(void);
#define N 100000
int a[N];
int main(void) {
  for (int i = 0; i < N; i++)
    a[i] = 3 * i + 1;
  unsigned x = __VERIFIER_nondet_uint();
  if (x < N)
    __VERIFIER_assert(a[x] == 3 * (int)x + 1);
  return 0;
}
